//
// Created by Lommy Reznak on 1/6/21.
//

#include "Interpreter.h"

expert_system::Value Interpreter::getValueByVarName(char vn) const
{
	try
	{
		return this->storage_.at(vn);
	}
	catch (const std::out_of_range &)
	{
		return expert_system::Value::Undefined;
	}
}

void Interpreter::setVarWithValue(expert_system::Value vl, char vn)
{
	this->storage_[vn] = vl;
}

