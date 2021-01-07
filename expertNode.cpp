//
// Created by Lommy Reznak on 1/6/21.
//

#include "expertNode.h"
#include <string>
#include "Interpreter.h"
#include <sstream>
#include <iostream>

extern Interpreter interpreter;
using expert_system::Value;

# define RIGHT(VAL) Value((VAL) & 1U)
# define LEFT(VAL) Value((unsigned(VAL) >> 1U) & 1U)
# define TOUNSIGNED(VALL, VALR) (unsigned(VALL) << 1U | unsigned(VALR))

///
/// Var
///

expert_system::Value expert_system::Var::eval() {
	return interpreter.getValueByVarName(this->Letter);
}

std::string expert_system::Var::to_string() {
	return std::string(1, this->Letter);
}

void expert_system::Var::evalAs(Value val) {
	if (interpreter.isVarSet(this->Letter)
		&& val == interpreter.getValueByVarName(this->Letter))
		return;

	if (interpreter.isVarSet(this->Letter)
		&& interpreter.getValueByVarName(this->Letter) != Value::Undefined)
	{
		std::stringstream ss;

		ss << "Trying to set " << this->Letter << " when it is already set " << int(val) << " " << int(interpreter.getValueByVarName(this->Letter));
		throw std::logic_error(ss.str());
	}

	interpreter.setVarWithValue(val, this->Letter);
	interpreter.commitChanges();
}

std::string expert_system::Var::to_reduced_string() {
	auto t = interpreter.getValueByVarName(this->Letter);

	if (t == Value::Undefined)
		return "Undefined";
	if (t == Value::True)
		return "True";
	return "False";
}

expert_system::Var::~Var()
= default;

///
/// Implication
///

expert_system::Value expert_system::Implication::eval() {
	if (this->val_ != Value::Undefined) return this->val_;

	auto leftVal = this->l_->eval();

	if (leftVal == Value::False)
		return Value::True;

	return expert_system::Value::False;
}

std::string expert_system::Implication::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " => " + "(" + r_->to_string() + ")";
	return ret;
}

void expert_system::Implication::evalAs(Value val) {
	if (this->val_ != Value::Undefined && this->val_ != val)
	{
		std::stringstream ss;
		ss << this->to_string() << " cannot be both true and false";
		throw std::logic_error(ss.str());
	}

	val_ = val;

	auto leftVal = this->l_->eval();
	auto rightVal = this->r_->eval();


	std::set<unsigned> newS;

	if (val == Value::True)
	{
		auto& t = solutions.at("Implication").at(Value::True);
		set_intersection(t.begin(), t.end(), solutions_.begin(), solutions_.end(),
						 std::inserter(newS, newS.begin()));
	}
	else if (val == Value::False)
	{
		auto &t = solutions.at("Implication").at(Value::False);
		set_intersection(t.begin(), t.end(), solutions_.begin(), solutions_.end(),
						 std::inserter(newS, newS.begin()));
	}

	if (newS.find(TOUNSIGNED(leftVal, rightVal)) != newS.end())
		return ;

	if (newS.empty())
	{
		std::stringstream ss;
		ss << "No solution for " << this->to_string();
		throw std::logic_error(ss.str());
	}

	std::set<Value> leftCandidates;
	std::set<Value> rightCandidates;

	for (auto slt : newS)
	{
		try {
			l_->evalAs(LEFT(slt));
			r_->evalAs(RIGHT(slt));

			solutions_ = newS;
			return;
		}
		catch (const std::exception &e) {
			std::cerr << "DEBUG: " << e.what() << std::endl;
		}
	}



	throw std::logic_error("Cannot set this");

}

std::string expert_system::Implication::to_reduced_string() {
	std::string ret = "(" + l_->to_reduced_string() + ")" + " => " + "(" + r_->to_reduced_string() + ")";
	return ret;
}

expert_system::Implication::~Implication() {
	if (interpreter.repeatDestroyer_[l_->to_string()] != l_)
		delete l_;
	if (interpreter.repeatDestroyer_[r_->to_string()] != r_)
		delete r_;
}

///
/// And
///

expert_system::Value expert_system::And::eval() {
	auto leftVal = this->l_->eval();
	auto rightVal = this->r_->eval();

	if (leftVal == Value::True && rightVal == Value::True)
		return Value::True;

	return expert_system::Value::False;
}

std::string expert_system::And::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " + " + "(" + r_->to_string() + ")";
	return ret;
}

void expert_system::And::evalAs(Value val) {
	if (val == Value::True)
	{
		this->l_->evalAs(Value::True);
		this->r_->evalAs(Value::True);
	}
	else if (val == Value::False)
	{
		auto lres = l_->eval();
		auto rres = r_->eval();

		if (lres == Value::True && rres == Value::True)
		{
			throw std::logic_error("Cannot");
		}
	}
}

std::string expert_system::And::to_reduced_string() {
	std::string ret = "(" + l_->to_reduced_string() + ")" + " + " + "(" + r_->to_reduced_string() + ")";
	return ret;
}

expert_system::And::~And() {
	if (interpreter.repeatDestroyer_[l_->to_string()] != l_)
		delete l_;
	if (interpreter.repeatDestroyer_[r_->to_string()] != r_)
		delete r_;
}

///
/// Or
///

expert_system::Value expert_system::Or::eval() {

	auto leftVal = this->l_->eval();
	auto rightEval = this->r_->eval();

	if (leftVal == Value::True || rightEval == Value::True)
		return Value::True;

	if (leftVal == Value::Undefined && rightEval == Value::Undefined)
		return Value::True;

	if (leftVal == Value::Undefined || rightEval == Value::Undefined)
		return Value::Undefined;

	return expert_system::Value::False;
}

std::string expert_system::Or::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " | " + "(" + r_->to_string() + ")";
	return ret;
}

void expert_system::Or::evalAs(Value val) {
	auto leftVal = this->l_->eval();
	auto rightEval = this->r_->eval();

	if (val == Value::False)
	{
		this->l_->evalAs(Value::False);
		this->r_->evalAs(Value::False);
	}
	else if (val == Value::Undefined)
	{
		this->l_->evalAs(Value::Undefined);
		this->r_->evalAs(Value::Undefined);
	}
	else if (val == Value::True)
	{
		std::set<unsigned> slt {1, 2, 3};

		std::set<unsigned> new_answer;

		set_intersection(slt.begin(),slt.end(),this->solutions_.begin(),this->solutions_.end(),
						 std::inserter(new_answer,new_answer.begin()));
	}

}

std::string expert_system::Or::to_reduced_string() {
	std::string ret = "(" + l_->to_reduced_string() + ")" + " | " + "(" + r_->to_reduced_string() + ")";
	return ret;
}

expert_system::Or::~Or() {
	if (interpreter.repeatDestroyer_[l_->to_string()] != l_)
		delete l_;
	if (interpreter.repeatDestroyer_[r_->to_string()] != r_)
		delete r_;
}

///
/// Not
///

expert_system::Value expert_system::Not::eval() {
	auto res = this->c_->eval();

	if (res == Value::True)
		return Value::False;
	if (res == Value::False)
		return Value::True;
	return Value::Undefined;
}

std::string expert_system::Not::to_string() {
	std::string ret = "!(" + c_->to_string() + ")";
	return ret;
}

void expert_system::Not::evalAs(Value val)
{
	if (val == Value::False)
		this->c_->evalAs(Value::True);
	else if (val == Value::True)
		this->c_->evalAs(Value::False);
	else
		this->c_->evalAs(Value::Undefined);
}

std::string expert_system::Not::to_reduced_string() {
	std::string ret = "!(" + c_->to_reduced_string() + ")";
	return ret;
}

expert_system::Not::~Not() {
	if (interpreter.repeatDestroyer_[c_->to_string()] != c_)
		delete c_;
}

///
/// Xor
///

expert_system::Value expert_system::Xor::eval() {
	auto leftVal = this->l_->eval();
	auto rightVal = this->r_->eval();

	if (leftVal == Value::Undefined || rightVal == Value::Undefined)
		return Value::Undefined;

	if (leftVal != rightVal)
		return Value::True;

	return expert_system::Value::False;
}

std::string expert_system::Xor::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " ^ " + "(" + r_->to_string() + ")";
	return ret;
}

void expert_system::Xor::evalAs(Value val) {
	{
		auto leftVal = this->l_->eval();
		if (leftVal != Value::Undefined)
		{
			if (leftVal == Value::False)
				this->r_->evalAs(Value::True);
			else if (leftVal == Value::True)
				this->r_->evalAs(Value::False);
		}
	}
	{
		auto rightVal = this->r_->eval();
		if (rightVal != Value::Undefined)
		{
			if (rightVal == Value::False)
				this->l_->evalAs(Value::True);
			else if (rightVal == Value::True)
				this->l_->evalAs(Value::False);
		}
	}
}

std::string expert_system::Xor::to_reduced_string() {
	std::string ret = "(" + l_->to_reduced_string() + ")" + " ^ " + "(" + r_->to_reduced_string() + ")";
	return ret;
}

expert_system::Xor::~Xor() {
	if (interpreter.repeatDestroyer_[l_->to_string()] != l_)
		delete l_;
	if (interpreter.repeatDestroyer_[r_->to_string()] != r_)
		delete r_;
}

///
/// Equ
///

expert_system::Value expert_system::Equ::eval() {
	auto leftVal = this->l_->eval();
	auto rightVal = this->r_->eval();

	if (leftVal == Value::Undefined || rightVal == Value::Undefined)
		return Value::Undefined;

	if (leftVal == rightVal)
		return Value::True;

	return expert_system::Value::False;
}

std::string expert_system::Equ::to_string() {
	std::string ret = "(" + l_->to_string() + ")" + " <=> " + "(" + r_->to_string() + ")";
	return ret;
}

void expert_system::Equ::evalAs(Value val) {
	auto leftVal = this->l_->eval();
	if (leftVal != Value::Undefined)
	{
		this->r_->evalAs(leftVal);
		return;
	}

	auto rightVal = this->r_->eval();
	if (rightVal != Value::Undefined)
		this->l_->evalAs(rightVal);
}

std::string expert_system::Equ::to_reduced_string() {
	std::string ret = "(" + l_->to_reduced_string() + ")" + " <=> " + "(" + r_->to_reduced_string() + ")";
	return ret;
}

expert_system::Equ::~Equ() {
	if (interpreter.repeatDestroyer_[l_->to_string()] != l_)
		delete l_;
	if (interpreter.repeatDestroyer_[r_->to_string()] != r_)
		delete r_;
}


///
/// Val
///

expert_system::Value expert_system::Val::eval() {
	return expert_system::Value::False;
}

std::string expert_system::Val::to_string() {
	return std::string();
}

void expert_system::Val::evalAs(Value v)
{
}

std::string expert_system::Val::to_reduced_string() {
	return std::string();
}