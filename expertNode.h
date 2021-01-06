//
// Created by Lommy Reznak on 1/6/21.
//

#ifndef EXPERT_SYSTEM_EXPERTNODE_H
#define EXPERT_SYSTEM_EXPERTNODE_H

#include <memory>

namespace expertNodes {
	enum class Value
	{
		False = 0,
		True,
		Undefined
	};

	class iExpertNode {
	public:
		virtual Value eval() = 0;
		virtual std::string to_string() = 0;
	};

	class Implication : public iExpertNode
	{
	private:
		iExpertNode *l_{}; iExpertNode *r_{};
	public:
		Implication(iExpertNode *l, iExpertNode *r) : l_{l}, r_{r}
		{}
		Value eval() override;
		std::string to_string() override;
	};

	class And : public iExpertNode
	{
	private:
		iExpertNode *l_{}; iExpertNode *r_{};
	public:
		And(iExpertNode *l, iExpertNode *r) : l_{l}, r_{r}
		{}
		Value eval() override;
		std::string to_string() override;
	};

	class Or : public iExpertNode
	{
	private:
		iExpertNode *l_{}; iExpertNode *r_{};
	public:
		Or(iExpertNode *l, iExpertNode *r) : l_{l}, r_{r}
		{}
		Value eval() override;
		std::string to_string() override;
	};

	class Not : public iExpertNode
	{
	private:
		iExpertNode *c_{};
	public:
		explicit Not(iExpertNode *c) : c_{c}
		{}
		Value eval() override;
		std::string to_string() override;
	};

	class Xor : public iExpertNode
	{
	private:
		iExpertNode *l_{}; iExpertNode *r_{};
	public:
		Xor(iExpertNode *l, iExpertNode *r) : l_{l}, r_{r}
		{}
		Value eval() override;
		std::string to_string() override;
	};

	class Equ : public iExpertNode
	{
	private:
		iExpertNode *l_{}; iExpertNode *r_{};
	public:
		Equ(iExpertNode *l, iExpertNode *r) : l_{l}, r_{r}
		{}
		Value eval() override;
		std::string to_string() override;
	};

	class Var : public iExpertNode
	{
	private:
		char Letter{};
	public:
		explicit Var(char c) : Letter{c}
		{}
		Value eval() override;
		std::string to_string() override;
	};

	class Val : public iExpertNode
	{
	private:
		Value val{};
	public:
		Value eval() override;
		std::string to_string() override;
	};

}




#endif //EXPERT_SYSTEM_EXPERTNODE_H
