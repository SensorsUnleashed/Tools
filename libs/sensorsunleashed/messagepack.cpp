#include "messagepack.h"
static bool buf_reader(cmp_ctx_t *ctx, void *data, uint32_t limit);
static uint32_t buf_writer(cmp_ctx_t* ctx, const void *data, uint32_t count);
QString cmpobjectToString(cmp_object_t obj);

suValue::suValue(cmp_object_t val){

    this->val.as = val.as;
    this->val.type = val.type;
}

suValue::suValue(QByteArray buffer){
    init(&buffer);
    cmp_read_object(&cmp, &val);

    //TODO: Check if value did change before emitting
    emit valueChanged();
}

void suValue::update(cmp_object_t val){
    this->val.as = val.as;
    this->val.type = val.type;

    //TODO: Check if value did change before emitting
    emit valueChanged();
}

int suValue::get(QByteArray* buffer){
    QByteArray b(10, 0);
    init(&b);

    cmp_write_object(&cmp, &val);

    int len = static_cast<int>((static_cast<char*>(cmp.buf) - b.data()));
    b.resize(len);
    buffer->append(b);
    return 0;
}

QString suValue::toString(){
    return cmpobjectToString(val);
}

void suValue::init(QByteArray* buffer){
    cmp_init(&cmp, buffer->data(), buf_reader, buf_writer);
}

static bool buf_reader(cmp_ctx_t *ctx, void *data, uint32_t limit) {

    uint8_t* dataptr = static_cast<uint8_t*>(data);
    uint8_t* bufptr = static_cast<uint8_t*>(ctx->buf);

    for(uint32_t i=0; i<limit; i++){
        *dataptr++ = *bufptr++;
    }

    data = dataptr;
    ctx->buf = bufptr;

    return true;
}

static uint32_t buf_writer(cmp_ctx_t* ctx, const void *data, uint32_t count){

    uint8_t* dataptr = (uint8_t*)(data);
    uint8_t* bufptr = static_cast<uint8_t*>(ctx->buf);

    for(uint32_t i=0; i<count; i++){
        *bufptr++ = *dataptr++;
    }
    data = dataptr;
    ctx->buf = bufptr;

    return count;
}

QString cmpobjectToString(cmp_object_t obj){
    QString result;
    switch(obj.type){
    case CMP_TYPE_POSITIVE_FIXNUM:
    case CMP_TYPE_UINT8:
        result = QString::number(obj.as.u8);
        break;
    case CMP_TYPE_NIL:
        result = "NIL";
        break;
    case CMP_TYPE_BOOLEAN:
        result = QString::number(obj.as.boolean);
        break;
    case CMP_TYPE_FLOAT:
        result = QString::number(obj.as.flt);
        break;
    case CMP_TYPE_DOUBLE:
        result = QString::number(obj.as.dbl);
        break;
    case CMP_TYPE_UINT16:
        result = QString::number(obj.as.u16);
        break;
    case CMP_TYPE_UINT32:
        result = QString::number(obj.as.u32);
        break;
    case CMP_TYPE_UINT64:
        result = QString::number(obj.as.u64);
        break;
    case CMP_TYPE_SINT8:
    case CMP_TYPE_NEGATIVE_FIXNUM:
        result = QString::number(obj.as.s8);
        break;
    case CMP_TYPE_SINT16:
        result = QString::number(obj.as.s16);
        break;
    case CMP_TYPE_SINT32:
        result = QString::number(obj.as.s32);
        break;
    case CMP_TYPE_SINT64:
        result = QString::number(obj.as.s64);
        break;
    default:
        result = "Not yet implemented!";
    }
        //    case CMP_TYPE_FIXMAP:
//        result["enum_str"] = "CMP_TYPE_FIXMAP";
//        break;
//    case CMP_TYPE_FIXARRAY:
//        result["enum_str"] = "CMP_TYPE_FIXARRAY";
//        for(uint32_t i=0; i<obj.as.array_size; i++){
//            //fixme: Only handles byte arrays
//            int32_t s;
//            cmp_read_int(cmp, &s);
//            arr.append(s);
//        }
//        result["value"] = arr;
//        break;
//    case CMP_TYPE_FIXSTR:
//        result["enum_str"] = "CMP_TYPE_FIXSTR";
//        str.reserve(size);
//        cmp->buf = static_cast<uint8_t*>(cmp->buf) - 1; //To read the string, we need to start at the size marker
//        if(!cmp_read_str(cmp, str.data(), reinterpret_cast<uint32_t*>(&size))) return QVariant(0);
//        str.resize(size);
//        result["value"] = QString::fromLatin1(str.data());
//        break;
//    case CMP_TYPE_BIN8:
//        result["enum_str"] = "CMP_TYPE_BIN8";
//        str.reserve(size);
//        cmp->buf = static_cast<uint8_t*>(cmp->buf) - 2;
//        if(!cmp_read_bin(cmp, str.data(), reinterpret_cast<uint32_t*>(&size))) return QVariant(0);
//        str.resize(size);
//        result["value"] = str;
//        break;
//    case CMP_TYPE_BIN16:
//        result["enum_str"] = "CMP_TYPE_BIN16";
//        break;
//    case CMP_TYPE_BIN32:
//        result["enum_str"] = "CMP_TYPE_BIN32";
//        break;
//    case CMP_TYPE_EXT8:
//        result["enum_str"] = "CMP_TYPE_EXT8";
//        break;
//    case CMP_TYPE_EXT16:
//        result["enum_str"] = "CMP_TYPE_EXT16";
//        break;
//    case CMP_TYPE_EXT32:
//        result["enum_str"] = "CMP_TYPE_EXT32";
//        break;
//    case CMP_TYPE_FIXEXT1:
//        result["enum_str"] = "CMP_TYPE_FIXEXT1";
//        break;
//    case CMP_TYPE_FIXEXT2:
//        result["enum_str"] = "CMP_TYPE_FIXEXT2";
//        break;
//    case CMP_TYPE_FIXEXT4:
//        result["enum_str"] = "CMP_TYPE_FIXEXT4";
//        break;
//    case CMP_TYPE_FIXEXT8:
//        result["enum_str"] = "CMP_TYPE_FIXEXT8";
//        break;
//    case CMP_TYPE_FIXEXT16:
//        result["enum_str"] = "CMP_TYPE_FIXEXT16";
//        break;
//    case CMP_TYPE_STR8:
//        result["enum_str"] = "CMP_TYPE_STR8";
//        break;
//    case CMP_TYPE_STR16:
//        result["enum_str"] = "CMP_TYPE_STR16";
//        break;
//    case CMP_TYPE_STR32:
//        result["enum_str"] = "CMP_TYPE_STR32";
//        break;
//    case CMP_TYPE_ARRAY16:
//        result["enum_str"] = "CMP_TYPE_ARRAY16";
//        break;
//    case CMP_TYPE_ARRAY32:
//        result["enum_str"] = "CMP_TYPE_ARRAY32";
//        break;
//    case CMP_TYPE_MAP16:
//        result["enum_str"] = "CMP_TYPE_MAP16";
//        break;
//    case CMP_TYPE_MAP32:
//        result["enum_str"] = "CMP_TYPE_MAP32";
//        break;
//    }
    return result;
}
