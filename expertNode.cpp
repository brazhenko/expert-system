//
// Created by Lommy Reznak on 1/6/21.
//

#include "expertNode.h"
#include <string>

expert_system::Value expert_system::Implication::eval() {
	auto leftVal = this->l_->eval();
	auto rightEval = this->r_->eval();

	return expert_system::Value::False;
}

std::string expert_system::Implication::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " => " + "(" + r_->to_string() + ")";
	return ret;
}

expert_system::Value expert_system::And::eval() {
	auto leftVal = this->l_->eval();
	auto rightEval = this->r_->eval();

	return expert_system::Value::False;
}

std::string expert_system::And::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " + " + "(" + r_->to_string() + ")";
	return ret;
}

expert_system::Value expert_system::Or::eval() {
	auto leftVal = this->l_->eval();
	auto rightEval = this->r_->eval();

	return expert_system::Value::False;
}

std::string expert_system::Or::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " | " + "(" + r_->to_string() + ")";
	return ret;
}

expert_system::Value expert_system::Not::eval() {


	return expert_system::Value::False;
}

std::string expert_system::Not::to_string() {
	std::string ret = "!(" + c_->to_string() + ")";
	return ret;
}

expert_system::Value expert_system::Xor::eval() {
	auto leftVal = this->l_->eval();
	auto rightEval = this->r_->eval();

	return expert_system::Value::False;
}

std::string expert_system::Xor::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " ^ " + "(" + r_->to_string() + ")";
	return ret;
}

expert_system::Value expert_system::Equ::eval() {
	auto leftVal = this->l_->eval();
	auto rightEval = this->r_->eval();

	return expert_system::Value::False;
}

std::string expert_system::Equ::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " <=> " + "(" + r_->to_string() + ")";
	return ret;
}

expert_system::Value expert_system::Var::eval() {


	return expert_system::Value::False;
}

std::string expert_system::Var::to_string() {
	return std::string(1, this->Letter);
}

expert_system::Value expert_system::Val::eval() {
	return expert_system::Value::False;
}

std::string expert_system::Val::to_string() {
	return std::string();
}
