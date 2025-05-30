#include <stdio.h>
#include <string.h>
#include "ncbind/ncbind.hpp"
#include <map>

#define NCB_MODULE_NAME TJS_W("varfile.dll")
#define BASENAME TJS_W("var")

static bool isDirectory(tTJSVariant &base) {
	return base.Type() == tvtObject && base.AsObjectNoAddRef() != NULL;
}

static bool isFile(const tTJSVariant &file) {
	return file.Type() == tvtOctet;
}

class VariantStream : public tTJSBinaryStream {

public:
	VariantStream(tTJSVariant &parent) : refCount(1), parent(parent), stream(0), cur(0) {};

	virtual ~VariantStream() {
		close();
	}

	bool open(const ttstr &name, tjs_uint32 flags) {
		close();
		this->name = name;

		if (flags == TJS_BS_READ) {
			parent.AsObjectClosureNoAddRef().PropGet(0, name.c_str(), NULL, &value, NULL);
			return isFile(value);
		}

        stream = new tTVPMemoryStream();

		if (flags == TJS_BS_UPDATE || flags == TJS_BS_APPEND) {
			parent.AsObjectClosureNoAddRef().PropGet(0, name.c_str(), NULL, &value, NULL);
			if (isFile(value)) {
				stream->Write(value.AsOctetNoAddRef()->GetData(), value.AsOctetNoAddRef()->GetLength());
				stream->Seek(0, flags == TJS_BS_UPDATE ? TJS_BS_SEEK_SET : TJS_BS_SEEK_END);
			}
		}
		return true;
	}
	
    virtual tjs_uint TJS_INTF_METHOD Read(void *pv, tjs_uint cb) {
		if (stream) {
			return stream->Read(pv, cb);
		} else {
			const tjs_uint8 *base = getBase();
			tTVInteger size = getSize() - cur;
			if (base && cb > 0 && size > 0) {
				if (cb > size) {
					cb = size;
				}
				memcpy(pv, base + cur, cb);
				cur += cb;
				return cb;
			} else {
				return 0;
			}
		}
	}

    virtual tjs_uint TJS_INTF_METHOD Write(const void *pv, tjs_uint cb) {
		if (stream) {
			return stream->Write(pv, cb);
		} else {
			return 0;
		}
	}

    virtual tjs_uint64 TJS_INTF_METHOD Seek(tjs_int64 dlibMove, tjs_int dwOrigin) {
		if (stream) {
			return stream->Seek(dlibMove, dwOrigin);
		} else {
			switch (dwOrigin) {
			case TJS_BS_SEEK_CUR:
				cur += dlibMove;
				break;
			case TJS_BS_SEEK_SET:
				cur = dlibMove;
				break;
			case TJS_BS_SEEK_END:
				cur = getSize();
				cur += dlibMove;
				break;
			}
			return cur;
		}
	}

    virtual tjs_uint64 TJS_INTF_METHOD GetSize() {
        if (stream) {
            return stream->GetSize();
        } else {
            return getSize();
        }
    }
	
protected:

	void close() {
		if (stream) {
			delete stream;
			stream = NULL;
		}
		value.Clear();
		cur = 0;
	}

	const tjs_uint8 *getBase() {
		return isFile(value) ? value.AsOctetNoAddRef()->GetData() : NULL;
	}

    virtual tjs_uint64 TJS_INTF_METHOD getSize() {
		return isFile(value) ? value.AsOctetNoAddRef()->GetLength() : 0;
	}

private:
	int refCount;
	tTJSVariant parent;
	ttstr name;
	tTJSVariant value;
	tTJSBinaryStream *stream;
	tTVInteger cur;
};

class GetLister : public tTJSDispatch
{

public:
	GetLister(iTVPStorageLister *lister) : lister(lister) {};

	virtual tjs_error TJS_INTF_METHOD FuncCall( // function invocation
												tjs_uint32 flag,			// calling flag
												const tjs_char * membername,// member name ( NULL for a default member )
												tjs_uint32 *hint,			// hint for the member name (in/out)
												tTJSVariant *result,		// result
												tjs_int numparams,			// number of parameters
												tTJSVariant **param,		// parameters
												iTJSDispatch2 *objthis		// object as "this"
												) {
		if (numparams > 1) {
			tTVInteger flag = param[1]->AsInteger();
			if (!(flag & TJS_HIDDENMEMBER) && isFile(*param[2])) {
				lister->Add(ttstr(param[0]->GetString()));
			}
		}
		if (result) {
			*result = true;
		}
		return TJS_S_OK;
	}

private:
	iTVPStorageLister *lister;
};


class VarStorage : public iTVPStorageMedia
{

public:
	VarStorage() : refCount(1) {
	}

	virtual void TJS_INTF_METHOD AddRef() {
		refCount++;
	};

	virtual void TJS_INTF_METHOD Release() {
		if (refCount == 1) {
			delete this;
		} else {
			refCount--;
		}
	};

	virtual void TJS_INTF_METHOD GetName(ttstr &name) {
		name = BASENAME;
	}

	virtual void TJS_INTF_METHOD NormalizeDomainName(ttstr &name) {
		
	}

	virtual void TJS_INTF_METHOD NormalizePathName(ttstr &name) {

	}

	virtual bool TJS_INTF_METHOD CheckExistentStorage(const ttstr &name) {
		return isFile(getFile(name));
	}

	virtual tTJSBinaryStream * TJS_INTF_METHOD Open(const ttstr & name, tjs_uint32 flags) {
		tTJSBinaryStream *ret = NULL;
		ttstr fname;
		tTJSVariant parent = getParentName(name, fname);
		if (isDirectory(parent) && fname.length() > 0) {
			VariantStream *stream = new VariantStream(parent);
			if (!stream->open(fname, flags)) {
				delete stream;
			} else {
				ret = stream;
			}
		}
		if (!ret) {
			TVPThrowExceptionMessage(TJS_W("cannot open memfile:%1"), name);
		}
		return ret;
	}

	virtual void TJS_INTF_METHOD GetListAt(const ttstr &name, iTVPStorageLister * lister) {
		tTJSVariant base = getFile(name);
		if (isDirectory(base)) {
			tTJSVariantClosure closure(new GetLister(lister));
			base.AsObjectClosureNoAddRef().EnumMembers(TJS_IGNOREPROP, &closure, NULL);
			closure.Release();
		}
	}

	virtual void TJS_INTF_METHOD GetLocallyAccessibleName(ttstr &name) {
		name = "";
	}

protected:

	virtual ~VarStorage() {
	}
	
	tTJSVariant getParentName(const ttstr &name, ttstr &fname) {
		const tjs_char *p = name.c_str();
		const tjs_char *q;
		if ((q = TJS_strchr(p, '/'))) {
			ttstr dname = ttstr(p, q-p);
			if (dname != TJS_W(".")) {
				TVPThrowExceptionMessage(TJS_W("no such domain:%1"), dname);
			}
		} else {
			TVPThrowExceptionMessage(TJS_W("invalid path:%1"), name);
		}
		ttstr path = ttstr(q+1);
		iTJSDispatch2 *global = TVPGetScriptDispatch();
		tTJSVariant base(global, global);
		while (path.length() > 0) {
			p = path.c_str();
			q = TJS_strchr(p, '/');
			if (q == NULL) {
				break;
			} else if (q == p) {
				base.Clear();
				break;
			} else {
				ttstr member = ttstr(p, q-p);
				tTJSVariant value;
				tTJSVariantClosure &o = base.AsObjectClosureNoAddRef();
				if (((o.IsInstanceOf(0, NULL, NULL, TJS_W("Array"), NULL) == TJS_S_TRUE &&
					  TJS_SUCCEEDED(o.PropGetByNum(0, (tjs_int)TJSStringToInteger(member.c_str()), &value, NULL))) ||
					 (TJS_SUCCEEDED(o.PropGet(0, member.c_str(), NULL, &value, NULL)))) && isDirectory(value)) {
					base = value;
					path = ttstr(q+1);
				} else {
					base.Clear();
					break;
				}
			}
		}
		fname = path;
		return base;
	}
	
	tTJSVariant getFile(const ttstr &name) {
		ttstr fname;
		tTJSVariant base = getParentName(name, fname);
		if (isDirectory(base) && fname.length() > 0) {
			tTJSVariant value;
			if (TJS_SUCCEEDED(base.AsObjectClosureNoAddRef().PropGet(0, fname.c_str(), NULL, &value, NULL))) {
				base = value;
			} else {
				base.Clear();
			}
		}
		return base;
	}

private:
	tjs_uint refCount;
};

VarStorage *var = NULL;

static void PreRegistCallback()
{
	if (var == NULL) {
		var = new VarStorage();
		TVPRegisterStorageMedia(var);
	}
}

static void PostUnregistCallback()
{
	if (var != NULL) {
		TVPUnregisterStorageMedia(var);
		var->Release();
		var = NULL;
	}
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
