#include "stdafx.h"
#include <lib/format/json.h>
#include <lib/utility/utility.h>

namespace {
const char *skipSpace(const char *data) {
	while (*data == ' ' || *data == '\t' || *data == '\r' || *data == '\n')  data++;
#ifdef	ENABLE_JSON_COMMENT
	while(*data == '/' && (*(data+1) == '*' || *(data+1) == '/')) {
		data++;
		if (*data == '/') {
			while(*data != '\n' && *data != '\0')  data++;
		} else {
			*data++;
			while(*data != '\0') {
				if (*data == '*' && *(data+1) == '/') {
					data += 2;
					break;
				}
				data++;
			}
		}
		while (*data == ' ' || *data == '\t' || *data == '\r' || *data == '\n')  data++;
	}
#endif
	return data;
}
}

namespace lib {
namespace format {

const char *JsonElement::copyValue(std::string *dst, const char *data) {
	*dst = "";
	while (*data != ' ' && *data != '\t' && *data != '\r' && *data != '\n' && *data != ',' && *data != '\0' && *data != '}' && *data != ']') {
#ifdef ENABLE_JSON_COMMENT
		if (*data == '/' && (*(data+1) == '*' || *(data+1) == '/')) {
			break;
		}
#endif
		*dst += *data;
		data++;
	}
	return data;
}

const char *JsonElement::copyString(std::string *dst, const char *data) {
	*dst = "";
	std::string buf;
	if (*data != '"') {
		// エラー
		setParseError();
		return data;
	}
	const char *p = data;
	p++;
	while (*p != '\0' && *p != '"') {
		if (*p == '\n') {
			// 文字列内に改行を認めない
			setParseError();
			return data;
		}
		if (*p == '\\' && (*(p+1) == '\\' || *(p+1) == '\"')) {
			buf += '\\';
			buf += *(p+1);
			p+=2;
		} else {
			buf += *p;
			p++;
		}
	}
	if (*p != '"') {
		// エラー
		setParseError();
		return data;
	}
	p++;

	utility::expansionEscape(dst, buf.c_str());

	return p;
}

const char *JsonElement::create(JsonElement **element, const char *data)
{
	const char *p = skipSpace(data);
	*element = NULL;
	if (*p == '{') {
		JsonObject *json_object = NEW("JsonElement::JsonObject") JsonObject();
		p = json_object->parse(p);
		*element = json_object;
	} else if (*p == '[') {
		JsonArray *json_array= NEW("JsonElement::JsonArray") JsonArray();
		p = json_array->parse(p);
		*element = json_array;
	} else if (*p == '"') {
		JsonString *json_string = NEW("JsonElement::JsonString") JsonString();
		std::string dst;
		p = copyString(&dst, p);
		if (isParseError()) {
			return data;
		}
		json_string->setValue(dst.c_str());
		*element = json_string;
	} else {
		std::string dst;
		p = copyValue(&dst, p);
		if (dst.compare("null") == 0) {
			JsonNull *json_null = NEW("JsonElement::JsonNull") JsonNull();
			*element = json_null;
		} else if (dst.compare("true") == 0) {
			JsonBool *json_bool = NEW("JsonElement::JsonBool") JsonBool(true);
			*element = json_bool;
		} else if (dst.compare("false") == 0) {
			JsonBool *json_bool = NEW("JsonElement::JsonBool") JsonBool(false);
			*element = json_bool;
		} else if (utility::isInt(dst.c_str())){
			JsonInt *json_int = NEW("JsonElement::JsonInt") JsonInt();
			json_int->setValue(utility::a2i(dst.c_str()));
			*element = json_int;
		} else if (utility::isFloat(dst.c_str())){
			JsonDouble *json_double = NEW("JsonElement::JsonDouble") JsonDouble();
			json_double->setValue(utility::a2f(dst.c_str()));
			*element = json_double;
		} else {
			setParseError();
			return data;
		}
	}

	if (!element || !(*element) || (*element)->isParseError()) {
		if (element) SAFE_DELETE(*element);
		setParseError();
		return p;
	}
	(*element)->setParent(this);

	return p;
}

const char *JsonElement::getString()
{
	if (isString()) {
		JsonString *obj = (JsonString*)this;
		return obj->getValue();
	}
	return "";
}

long long JsonElement::getInt()
{
	if (isInt()) {
		JsonInt *obj = (JsonInt*)this;
		return obj->getValue();
	}
	return 0;
}

long double JsonElement::getDouble()
{
	if (isDouble()) {
		JsonDouble *obj = (JsonDouble*)this;
		return obj->getValue();
	}
	return 0.f;
}

bool JsonElement::getBool()
{
	if (isBool()) {
		JsonBool *obj = (JsonBool*)this;
		return obj->getValue();
	}
	return false;
}

JsonArray *JsonElement::getArray()
{
	if (isArray()) {
		JsonArray *obj = (JsonArray*)this;
		return obj;
	}
	return NULL;
}

JsonObject *JsonElement::getObject()
{
	if (isObject()) {
		JsonObject *obj = (JsonObject*)this;
		return obj;
	}
	return NULL;
}

void JsonString::dump(std::string *out)
{
	std::string t;
	utility::contractionEscape(&t, value_.c_str());
	*out += '"';
	*out += t;
	*out += '"';
}

void JsonArray::dump(std::string *out)
{
	*out += '[';
	for (ElementList::iterator it = element_list_.begin(); it != element_list_.end(); ++it) {
		if (it != element_list_.begin())  *out += ',';
		(*it)->dump(out);
	}
	*out += ']';
}

const char *JsonArray::parse(const char *data)
{
	const char *p = skipSpace(data);
	if (*p != '[') {
		// エラー
		setParseError();
		return p;
	}
	p++;
	p = skipSpace(p);
	if (*p == ']') {
		p++;
		return p;
	}
	while (true) {
		JsonElement *element = NULL;
		p = skipSpace(p);
		p = create(&element, p);
		if (isParseError()) {
			return p;
		}
		addElement(element);
		p = skipSpace(p);
		if (*p == ']') {
			p++;
			break;
		}
		if (*p != ',') {
			setParseError();
			return p;
		}
		p++;
	}

	return p;
}

void JsonObject::dump(std::string *out)
{
	*out += '{';
	for (ElementList::iterator it = element_list_.begin(); it != element_list_.end(); ++it) {
		if (it != element_list_.begin())  *out += ',';
		*out += "\"" + it->first + "\":";
		it->second->dump(out);
	}
	*out += '}';
}

const char *JsonObject::parse(const char *data)
{
	const char *p = skipSpace(data);
	if (*p != '{') {
		// エラー
		setParseError();
		return p;
	}
	p++;
	p = skipSpace(p);
	if (*p == '}') {
		p++;
		return p;
	}
	while (true) {
		std::string name;
		JsonElement *element = NULL;
		const char *_p = skipSpace(p);
		p = copyString(&name, _p);
		if (isParseError()) {
			return _p;
		}
		p = skipSpace(p);
		if (*p != ':') {
			setParseError();
			return _p;
		}
		p++;
		p = create(&element, p);
		if (isParseError()) {
			return p;
		}
		addElement(name.c_str(), element);
		p = skipSpace(p);
		if (*p == '}') {
			break;
		}
		if (*p != ',') {
			setParseError();
			return p;
		}
		p++;
	}
	p++;

	return p;
}

const char *JsonObject::getKey(int index)
{
	if (0 <= index && index < static_cast<int>(element_list_.size())) {
		return element_list_[index].first.c_str();
	}
	return NULL;
}

void JsonObject::addElement(const char *name, JsonElement *element)
{
	removeElement(name);
	element_list_.push_back(ElementData(name, element));
}

void JsonObject::removeElement(const char *name)
{
	for (ElementList::iterator it = element_list_.begin(); it != element_list_.end(); ++it) {
		if (it->first == name) {
			SAFE_DELETE(it->second);
			element_list_.erase(it);
		}
	}
}

JsonElement *JsonObject::getElement(const char *name)
{
	for (ElementList::iterator it = element_list_.begin(); it != element_list_.end(); ++it) {
		if (it->first == name) {
			return it->second;
		}
	}
	return NULL;
}

const char *JsonDocument::dump(std::string *out)
{
	*out = "";
	if (root_element_) {
		root_element_->dump(out);
	}
	return out->c_str();
}


void JsonDocument::parse(const char *data)
{
	parse_error_ = false;
	last_parse_pos_ = NULL;
	error_line_no_ = -1;
	SAFE_DELETE(root_element_);
	if (!data) {
		parse_error_ = true;
		return;
	}
	const char *p = skipSpace(data);
	if (*p == '{') {
		JsonObject *json_object = NEW("JsonReader::JsonObject") JsonObject();
		last_parse_pos_ = json_object->parse(p);
		root_element_ = json_object;
	} else if (*p == '[') {
		JsonArray *json_array = NEW("JsonReader::JsonArray") JsonArray();
		last_parse_pos_ = json_array->parse(p);
		root_element_ = json_array;
	} else {
		parse_error_ = true;
	}
	if (last_parse_pos_ && !isParseError()) {
		last_parse_pos_ = skipSpace(last_parse_pos_);
		if (*last_parse_pos_ != '\0') {
			parse_error_ = true;
		}
	}
	if (root_element_ && root_element_->isParseError()) {
		parse_error_ = true;
	}
	if(parse_error_ && last_parse_pos_) {
		// 行番号
		p = data;
		error_line_no_ = 1;
		while (p < last_parse_pos_) {
			if (*p == '\n')  error_line_no_++;
			p++;
		}
	}
}

} // namespace format
} // namespace lib
