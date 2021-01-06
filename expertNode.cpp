//
// Created by Lommy Reznak on 1/6/21.
//

#include "expertNode.h"
#include <string>

expertNodes::Value expertNodes::Implication::eval() {
	return expertNodes::Value::False;
}

std::string expertNodes::Implication::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " => " + "(" + r_->to_string() + ")";
	return ret;
}

expertNodes::Value expertNodes::And::eval() {
	return expertNodes::Value::False;
}

std::string expertNodes::And::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " + " + "(" + r_->to_string() + ")";
	return ret;
}

expertNodes::Value expertNodes::Or::eval() {
	return expertNodes::Value::False;
}

std::string expertNodes::Or::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " | " + "(" + r_->to_string() + ")";
	return ret;
}

expertNodes::Value expertNodes::Not::eval() {
	return expertNodes::Value::False;
}

std::string expertNodes::Not::to_string() {
	std::string ret = "!(" + c_->to_string() + ")";
	return ret;
}

expertNodes::Value expertNodes::Xor::eval() {
	return expertNodes::Value::False;
}

std::string expertNodes::Xor::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " ^ " + "(" + r_->to_string() + ")";
	return ret;
}

expertNodes::Value expertNodes::Equ::eval() {
	return expertNodes::Value::False;
}

std::string expertNodes::Equ::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " <=> " + "(" + r_->to_string() + ")";
	return ret;
}

expertNodes::Value expertNodes::Var::eval() {
	return expertNodes::Value::False;
}

std::string expertNodes::Var::to_string() {
	return std::string(1, this->Letter);
}

expertNodes::Value expertNodes::Val::eval() {
	return expertNodes::Value::False;
}

std::string expertNodes::Val::to_string() {
	return std::string();
}
