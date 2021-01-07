#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
//
// Created by Lommy Reznak on 1/6/21.
//

#ifndef EXPERT_SYSTEM_EXPERTNODE_H
#define EXPERT_SYSTEM_EXPERTNODE_H

#include <memory>
#include <set>
#include <map>
#include <string>

namespace expert_system {

	constexpr unsigned false_ = 0b0;
	constexpr unsigned true_ = 0b1;

	enum class Value
	{
		False = 0,
		True,
		Undefined
	};

	static const std::map<std::string, std::map<Value, std::set<unsigned>>> solutions = {
			{
					"Implication", {
										   {
												   Value::True, {0b00, 0b01, 0b11}
										   },
										   {
												   Value::False, {0b10}
										   },
								   }
			},

			{
					"And", {
										   {
												   Value::True, {0b11}
										   },
										   {
												   Value::False, {0b00, 0b01, 0b10}
										   },
								   }
			},
			{
					"Or", {
										   {
												   Value::True, {0b01, 0b10, 0b11}
										   },
										   {
												   Value::False, {0b00}
										   },
								   }
			},
			{
					"Xor", {
										   {
												   Value::True, {0b01, 0b10}
										   },
										   {
												   Value::False, {0b00, 0b11}
										   },
								   }
			},
			{
					"Equ", {
										   {
												   Value::True, {0b00, 0b11}
										   },
										   {
												   Value::False, {0b10, 0b01}
										   },
								   }
			},
			{
					"Not", {
										   {
												   Value::True, {0b0}
										   },
										   {
												   Value::False, {0b1}
										   },
								   }
			},
	};

	Value getValueByVarName(char name);

	void setVarByValue(char name, Value value);

	class iExpertNode {
	protected:
		Value val_ = Value::Undefined;
	public:
		virtual Value eval() = 0;
		virtual void evalAs(Value val) = 0;
		virtual std::string to_string() = 0;
		virtual std::string to_reduced_string() = 0;
		virtual ~iExpertNode() = default;
	};

	class Implication final : public  iExpertNode
	{
	private:
		iExpertNode *l_{}; iExpertNode *r_{};
		std::set<unsigned> solutions_{0, 1, 2, 3};
	public:
		Implication(iExpertNode *l, iExpertNode *r) : l_{l}, r_{r}
		{}
		Value eval() override;
		void evalAs(Value val) override;
		std::string to_string() override;
		std::string to_reduced_string() override;
		~Implication() override;
	};

	class And final : public iExpertNode
	{
	private:
		iExpertNode *l_{}; iExpertNode *r_{};
		std::set<unsigned> solutions_{0, 1, 2, 3};
	public:
		And(iExpertNode *l, iExpertNode *r) : l_{l}, r_{r}
		{}
		Value eval() override;
		void evalAs(Value val) override;
		std::string to_string() override;
		std::string to_reduced_string() override;
		~And() override;
	};

	class Or final  : public iExpertNode
	{
	private:
		iExpertNode *l_{}; iExpertNode *r_{};
		std::set<unsigned> solutions_{0b00, 0b01, 0b10, 0b11};
	public:
		Or(iExpertNode *l, iExpertNode *r) : l_{l}, r_{r}
		{}
		Value eval() override;
		void evalAs(Value val) override;
		std::string to_string() override;
		std::string to_reduced_string() override;
		~Or() override;
	};

	class Not final  : public iExpertNode
	{
	private:
		iExpertNode *c_{};
		std::set<unsigned> solutions_{0b0, 0b1};

	public:
		explicit Not(iExpertNode *c) : c_{c}
		{}
		Value eval() override;
		void evalAs(Value val) override;
		std::string to_string() override;
		std::string to_reduced_string() override;
		~Not() override;
	};

	class Xor final  : public iExpertNode
	{
	private:
		iExpertNode *l_{}; iExpertNode *r_{};
		std::set<unsigned> solutions_{0b00, 0b01, 0b10, 0b11};
	public:
		Xor(iExpertNode *l, iExpertNode *r) : l_{l}, r_{r}
		{}
		Value eval() override;
		void evalAs(Value val) override;
		std::string to_string() override;
		std::string to_reduced_string() override;
		~Xor() override;
	};

	class Equ final : public iExpertNode
	{
	private:
		iExpertNode *l_{}; iExpertNode *r_{};
		std::set<unsigned> solutions_{0b00, 0b01, 0b10, 0b11};
	public:
		Equ(iExpertNode *l, iExpertNode *r) : l_{l}, r_{r}
		{}
		Value eval() override;
		void evalAs(Value val) override;
		std::string to_string() override;
		std::string to_reduced_string() override;
		~Equ() override;
	};

	class Var final : public iExpertNode
	{
	private:
		char Letter{};
		std::set<unsigned> solutions_{0b0, 0b1};
	public:
		explicit Var(char c) : Letter{c}
		{
			val_ = Value::False;
		}
		char varName() const { return Letter; }
		Value eval() override;
		void evalAs(Value val) override;
		std::string to_string() override;
		std::string to_reduced_string() override;
		~Var() override;
	};

	class Val final : public iExpertNode
	{
	private:
		Value val{};
	public:
		Value eval() override;
		void evalAs(Value v) override;
		std::string to_string() override;
		std::string to_reduced_string() override;
	};

}


#endif //EXPERT_SYSTEM_EXPERTNODE_H

#pragma clang diagnostic pop