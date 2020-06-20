/*
 * MIT License
 * 
 * Copyright (c) 2020 Size Zheng

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include "IRPrinter.h"

namespace Boost {

namespace Internal {


std::string IRPrinter::print(const Expr &expr) {
    oss.str("");
    expr.visit_expr(this);
    return oss.str();
}


std::string IRPrinter::print(const Stmt &stmt) {
    oss.str("");
    stmt.visit_stmt(this);
    return oss.str();
}


std::string IRPrinter::print(const Group &group) {
    oss.str("");
    group.visit_group(this);
    return oss.str();
}

// Modified
void IRPrinter::visit(Ref<const IntImm> op) {
	oss << op->value();
}

// Modified
void IRPrinter::visit(Ref<const UIntImm> op) {
	oss << op->value();
}

// Modified
void IRPrinter::visit(Ref<const FloatImm> op) {
	oss << op->value() << ".0";
}

// Modified
void IRPrinter::visit(Ref<const StringImm> op) {
	oss << op->value();
}


void IRPrinter::visit(Ref<const Unary> op) {
    if (op->op_type == UnaryOpType::Neg) {
        oss << "-";
    } else if (op->op_type == UnaryOpType::Not) {
        oss << "!";
	}
	else if (op->op_type == UnaryOpType::Bracket) {
		oss << "(";
	}
    (op->a).visit_expr(this);
	if (op->op_type == UnaryOpType::Bracket) {
		oss << ")";
	}
}


void IRPrinter::visit(Ref<const Binary> op) {
    (op->a).visit_expr(this);
    if (op->op_type == BinaryOpType::Add) {
        oss << " + ";
    } else if (op->op_type == BinaryOpType::Sub) {
        oss << " - ";
    } else if (op->op_type == BinaryOpType::Mul) {
        oss << " * ";
    } else if (op->op_type == BinaryOpType::Div) {
        oss << " / ";
    } else if (op->op_type == BinaryOpType::Mod) {
        oss << " % ";
    } else if (op->op_type == BinaryOpType::And) {
        oss << " && ";
    } else if (op->op_type == BinaryOpType::Or) {
        oss << " || ";
    }
    (op->b).visit_expr(this);
}


void IRPrinter::visit(Ref<const Compare> op) {
    (op->a).visit_expr(this);
    if (op->op_type == CompareOpType::LT) {
        oss << " < ";
    } else if (op->op_type == CompareOpType::LE) {
        oss << " <= ";
    } else if (op->op_type == CompareOpType::EQ) {
        oss << " == ";
    } else if (op->op_type == CompareOpType::GE) {
        oss << " >= ";
    } else if (op->op_type == CompareOpType::GT) {
        oss << " > ";
    } else if (op->op_type == CompareOpType::NE) {
        oss << " != ";
    }
    (op->b).visit_expr(this);
}


// Modified
void IRPrinter::visit(Ref<const Select> op) {
    oss << "((";
    (op->cond).visit_expr(this);
    oss << ") ? (";
    (op->true_value).visit_expr(this);
    oss << ") : (";
    (op->false_value).visit_expr(this);
    oss << "))";
}


void IRPrinter::visit(Ref<const Call> op) {
    oss << "call_";
    if (op->call_type == CallType::Pure) {
        oss << "pure";
    } else if (op->call_type == CallType::SideEffect) {
        oss << "side_effect";
    };
    oss << "(" << op->func_name;
    for (size_t i = 0; i < op->args.size(); ++i) {
        oss << ", ";
        op->args[i].visit_expr(this);
    }
    oss << ")";
}


// Modified
void IRPrinter::visit(Ref<const Cast> op) {
    oss << "(" << op->new_type << ")(";
    (op->val).visit_expr(this);
    oss << ")";
}


void IRPrinter::visit(Ref<const Ramp> op) {
    oss << "ramp(";
    (op->base).visit_expr(this);
    oss << ", " << op->stride << ", " << op->lanes << ")";
}

// Modified
void IRPrinter::visit(Ref<const Var> op) {
	bool is_scalar = false;
	if (op->shape.size() == 1 && op->shape[0] == 1)
		is_scalar = true;

    if (print_arg) {
		
		if (op->type().is_int())
			oss << "int ";
		else if (op->type().is_float())
			oss << "float ";

		if (is_scalar)
			oss << "&" << op->name;
		else {
			oss << "(&" << op->name << ")";
	        oss << "[";
	        for (size_t i = 0; i < op->shape.size(); ++i) {
	            oss << op->shape[i];
	            if (i < op->shape.size() - 1) {
	                oss << "][";
	            }
	        }
	        oss << "]";
		}
    } else {
		oss << op->name;
		if (!is_scalar) {
			oss << "[";
			for (size_t i = 0; i < op->args.size(); ++i) {
				op->args[i].visit_expr(this);
				if (i < op->args.size() - 1) {
					oss << "][";
				}
			}
			oss << "]";
		}
    }
}

// Modified
void IRPrinter::visit(Ref<const Dom> op) {
	static int count = 0;
	if (!count)
		(op->begin).visit_expr(this);
	else
		(op->extent).visit_expr(this);
	count = (count + 1) % 2;
	
}


// Modified
void IRPrinter::visit(Ref<const Index> op) {
	if (print_range) {
	    oss << "(";
	    if (op->type().is_int()) {	
			oss << "int ";
	    } else if (op->type().is_float()) {
			oss << "float ";
	    }
		oss << op->name << "=";
	
		(op->dom).visit_expr(this);
			oss << "; " << op->name << "<";
		
		(op->dom).visit_expr(this);
		oss << "; ";
			oss << "++" << op->name << ") ";
		
	}
	else oss << op->name;
}


void IRPrinter::visit(Ref<const LoopNest> op) {
    print_range = true;
    for (auto index : op->index_list) {
        print_indent();
        oss << "for ";
        index.visit_expr(this);
        oss << "{\n";
        enter();
    }
    print_range = false;
    for (auto body : op->body_list) {
        body.visit_stmt(this);
    }
    for (auto index : op->index_list) {
        exit();
        print_indent();
        oss << "}\n";
    }
}


void IRPrinter::visit(Ref<const IfThenElse> op) {
    print_indent();
    oss << "if (";
    (op->cond).visit_expr(this);
    oss << ") {\n";
    enter();
    (op->true_case).visit_stmt(this);
    exit();
    print_indent();
    oss << "} else {\n";
    enter();
    (op->false_case).visit_stmt(this);
    exit();
    print_indent();
    oss << "}\n";
}

// Modified
void IRPrinter::visit(Ref<const Move> op) {
    print_indent();
    (op->dst).visit_expr(this);
	oss << " = ";
    (op->src).visit_expr(this);
    oss << ";\n";
}

// Modified
void IRPrinter::visit(Ref<const Kernel> op) {
    print_indent();
    oss << "void " << op->name << "(";

    print_arg = true;
    for (size_t i = 0; i < op->inputs.size(); ++i) {
        op->inputs[i].visit_expr(this);
        if (i < op->inputs.size() - 1) {
            oss << ", ";
        }
    }
    for (size_t i = 0; i < op->outputs.size(); ++i) {
        if (!(op->inputs.size() == 0 && i == 0))
			oss << ", ";
        op->outputs[i].visit_expr(this);
    }
    print_arg = false;
    oss << ") {\n";
	
    enter();
    for (auto stmt : op->stmt_list) {
        stmt.visit_stmt(this);
    }
    exit();
    oss << "}\n";
}


}  // namespace Internal

}  // namespace Boost
