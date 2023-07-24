#include "ChakraUtil.h"
#include "Util.h"
#include "client/Latite.h"

void Chakra::SetPropertyString(JsValueRef ref, std::wstring name, std::wstring value, bool strict) {
	JsPropertyIdRef propId;
	JS::JsGetPropertyIdFromName(name.c_str(), &propId);

	JsValueRef strRef;
	JS::JsCreateStringUtf16((const uint16_t*)value.c_str(), value.size(), &strRef);
	JS::JsSetProperty(ref, propId, strRef, strict);

	JS::JsRelease(strRef, nullptr);
}

void Chakra::SetPropertyNumber(JsValueRef ref, std::wstring name, double value, bool strict) {
	JsPropertyIdRef propId;
	JS::JsGetPropertyIdFromName(name.c_str(), &propId);

	JsValueRef num;
	JS::JsDoubleToNumber(value, &num);
	JS::JsSetProperty(ref, propId, num, strict);

	JS::JsRelease(num, nullptr);
}

void Chakra::SetPropertyBool(JsValueRef ref, std::wstring name, bool value, bool strict) {
	JsPropertyIdRef propId;
	JS::JsGetPropertyIdFromName(name.c_str(), &propId);

	JsValueRef num;
	JS::JsBoolToBoolean(value, &num);
	JS::JsSetProperty(ref, propId, num, strict);

	JS::JsRelease(num, nullptr);
}

void Chakra::DefineFunc(JsValueRef obj, JsNativeFunction func, std::wstring name, void* state) {
	JsPropertyIdRef propId;
	JS::JsGetPropertyIdFromName(name.c_str(), &propId);

	JsValueRef vf;
	JS::JsCreateFunction(func, state, &vf);
	JS::JsSetProperty(obj, propId, vf, true);

	JS::JsRelease(vf, nullptr);
}


std::wstring Chakra::GetStringProperty(JsValueRef ref, std::wstring name) {
	JsPropertyIdRef nameId;
	JS::JsGetPropertyIdFromName(name.c_str(), &nameId);

	JsValueRef val;
	auto err = JS::JsGetProperty(ref, nameId, &val);
	if (err != JsNoError) {
		return L"";
	}

	const wchar_t* str;
	size_t len;
	err = JS::JsStringToPointer(val, &str, &len);

	if (err != JsNoError) {
		Release(val);
		return L"";
	}

	Release(val);
	return std::wstring(str);
}

bool Chakra::GetBoolProperty(JsValueRef ref, std::wstring name) {
	JsPropertyIdRef nameId;
	JS::JsGetPropertyIdFromName(name.c_str(), &nameId);

	JsValueRef val;
	JS::JsGetProperty(ref, nameId, &val);

	bool b;
	JS::JsBooleanToBool(val, &b);

	Release(val);
	return b;
}

double Chakra::GetNumberProperty(JsValueRef ref, std::wstring name) {
	JsPropertyIdRef nameId;
	JS::JsGetPropertyIdFromName(name.c_str(), &nameId);

	JsValueRef val;
	JS::JsGetProperty(ref, nameId, &val);

	double b;
	JS::JsNumberToDouble(val, &b);
	Release(val);
	return b;
}

std::wstring Chakra::ToString(JsValueRef ref) {
	JsValueRef r2;
	JS::JsConvertValueToString(ref, &r2);
	const wchar_t* str;
	size_t len;
	JS::JsStringToPointer(r2, &str, &len);
	std::wstring wstr(str);
	Release(r2);
	return wstr;
}

double Chakra::GetNumber(JsValueRef ref) {
	double db;
	JS::JsNumberToDouble(ref, &db);
	return db;
}

bool Chakra::GetBool(JsValueRef ref) {
	bool b;
	JS::JsBooleanToBool(ref, &b);
	return b;
}

JsValueRef Chakra::GetNull() {
	JsValueRef ret;
	JS::JsGetNullValue(&ret);
	return ret;
}

d2d::Rect Chakra::GetRectFromJs(JsValueRef obj) {
	return { static_cast<float>(Chakra::GetNumberProperty(obj, L"left")), static_cast<float>(Chakra::GetNumberProperty(obj, L"top")),
	static_cast<float>(Chakra::GetNumberProperty(obj, L"right")), static_cast<float>(Chakra::GetNumberProperty(obj, L"bottom")) };
}

Vec2 Chakra::GetVec2FromJs(JsValueRef obj) {
	return { static_cast<float>(Chakra::GetNumberProperty(obj, L"x")), static_cast<float>(Chakra::GetNumberProperty(obj, L"y")) };
}

Vec3 Chakra::GetVec3FromJs(JsValueRef obj) {
	return { static_cast<float>(Chakra::GetNumberProperty(obj, L"x")), static_cast<float>(Chakra::GetNumberProperty(obj, L"y")), static_cast<float>(Chakra::GetNumberProperty(obj, L"z")) };
}

d2d::Color Chakra::GetColorFromJs(JsValueRef obj) {
	d2d::Color col;

	JsValueRef zero;
	JsValueRef one;
	JsValueRef two;
	JsValueRef three;

	JS::JsDoubleToNumber(0.0, &zero);
	JS::JsDoubleToNumber(1.0, &one);
	JS::JsDoubleToNumber(2.0, &two);
	JS::JsDoubleToNumber(3.0, &three);

	JsValueRef z;
	JsValueRef o;
	JsValueRef t;
	JsValueRef th;

	JS::JsGetIndexedProperty(obj, zero, &z);
	JS::JsGetIndexedProperty(obj, one, &o);
	JS::JsGetIndexedProperty(obj, two, &t);
	JS::JsGetIndexedProperty(obj, three, &th);

	col.r = static_cast<float>(Chakra::GetNumber(z));
	col.g = static_cast<float>(Chakra::GetNumber(o));
	col.b = static_cast<float>(Chakra::GetNumber(t));
	col.a = static_cast<float>(Chakra::GetNumber(th));


	Chakra::Release(zero);
	Chakra::Release(one);
	Chakra::Release(two);
	Chakra::Release(three);

	Chakra::Release(z);
	Chakra::Release(o);
	Chakra::Release(t);
	Chakra::Release(th);
	return col;
}

Chakra::Result Chakra::VerifyParameters(std::initializer_list<ParamContainer> params) {
	size_t count = 0;
	for (auto const& param : params) {
		count++;
		JsValueType trueType;
		JS::JsGetValueType(param.val, &trueType);
		if (trueType != param.type) {
			std::wstring wstr = L"Parameter " + std::to_wstring(count) + L" must be of correct type (has " + Chakra::GetTypeName(trueType) + L", needs " + Chakra::GetTypeName(param.type) + L")";
			return { false,wstr };
		}
	}
	return { true,L"" };
}

std::wstring Chakra::GetString(JsValueRef ref) {
	const wchar_t* str;
	size_t sz;
	JS::JsStringToPointer(ref, &str, &sz);
	return std::wstring(str);
}

void Chakra::ThrowError(std::wstring message) {
	JsValueRef errorValue;
	JsValueRef errorMessageValue;

	JS::JsCreateStringUtf16((const uint16_t*)message.c_str(), message.size(), &errorMessageValue);
	JS::JsCreateError(errorMessageValue, &errorValue);
	JS::JsSetException(errorValue);
}

void Chakra::Release(JsValueRef ref) {
	JS::JsRelease(ref, nullptr);
}

JsValueRef Chakra::GetUndefined() {
	JsValueRef ret;
	JS::JsGetUndefinedValue(&ret);
	return ret;
}

Chakra::Result Chakra::VerifyArgCount(unsigned short has, unsigned short expected, bool autoThrow) {
	if (has != expected) {
		auto ws = L"Argument count must be " + std::to_wstring(expected - 1);
		if (autoThrow) Chakra::ThrowError(ws);
		return { false, ws };
	}
	return { true, L"" };
}

JsValueRef Chakra::GetTrue() {
	JsValueRef ret;
	JS::JsGetTrueValue(&ret);
	return ret;
}

JsValueRef Chakra::GetFalse() {
	JsValueRef ret;
	JS::JsGetFalseValue(&ret);
	return ret;
}

std::wstring Chakra::GetTypeName(JsValueType type) {
	switch (type) {
	case JsValueType::JsUndefined:
		return L"undefined";
	case JsValueType::JsNull:
		return L"null";
	case JsValueType::JsNumber:
		return L"number";
	case JsValueType::JsString:
		return L"string";
	case JsValueType::JsObject:
		return L"object";
	case JsValueType::JsFunction:
		return L"function";
	case JsValueType::JsArray:
		return L"array";
	case JsValueType::JsError:
		return L"error";
	case JsValueType::JsBoolean:
		return L"boolean";
	case JsValueType::JsSymbol:
		return L"symbol";
	default:
		return L"<other type>";
	}
}