#pragma once
#include "AMSBaseVisitor.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>

// ============================================================
// Evaluator : Interpreted Execution Mode (ams run)
// Walks the AST and simulates monitoring rule evaluation
// ============================================================
class Evaluator : public AMSBaseVisitor {
public:

    // ---- Entry Point ----
    virtual std::any visitProgram(AMSParser::ProgramContext *ctx) override {
        std::cout << "========================================" << std::endl;
        std::cout << " AutomonScript Engine : Interpreter Mode" << std::endl;
        std::cout << "========================================" << std::endl;
        return visitChildren(ctx);
    }

    // ---- MONITOR "filename" ----
    virtual std::any visitMonitorStmt(AMSParser::MonitorStmtContext *ctx) override {
        std::string source = stripQuotes(ctx->STRING()->getText());
        std::cout << "[MONITOR] Registered data source: " << source << std::endl;
        return nullptr;
    }

    // ---- WATCH type "filename" ----
    virtual std::any visitWatchStmt(AMSParser::WatchStmtContext *ctx) override {
        std::string type = ctx->ID()->getText();
        std::string source = stripQuotes(ctx->STRING()->getText());
        std::cout << "[WATCH] Registered " << type << " watcher: " << source << std::endl;
        return nullptr;
    }

    // ---- SET variable = value ----
    virtual std::any visitSetStmt(AMSParser::SetStmtContext *ctx) override {
        std::string varName = ctx->ID()->getText();
        std::string val = ctx->value()->getText();
        // Strip quotes if string value
        if (val.length() >= 2 && val.front() == '"' && val.back() == '"') {
            val = val.substr(1, val.length() - 2);
        }
        variables[varName] = val;
        std::cout << "[SET] " << varName << " = " << val << std::endl;
        return nullptr;
    }

    // ---- RULE RuleName [SEVERITY level] ... END ----
    virtual std::any visitRuleBlock(AMSParser::RuleBlockContext *ctx) override {
        std::string ruleName = ctx->ID()->getText();
        std::cout << "\n[RULE] Evaluating rule: " << ruleName;

        // Handle optional SEVERITY
        if (ctx->severity()) {
            visit(ctx->severity());
        }
        std::cout << std::endl;

        // Visit WHEN clauses
        for (auto wc : ctx->whenClause()) {
            visit(wc);
        }

        // Visit optional IF clause
        if (ctx->ifClause()) {
            visit(ctx->ifClause());
        }

        // Visit DO block
        visit(ctx->doBlock());

        return nullptr;
    }

    // ---- SEVERITY level ----
    virtual std::any visitSeverity(AMSParser::SeverityContext *ctx) override {
        std::string level = ctx->severityLevel()->getText();
        std::cout << " [SEVERITY: " << level << "]";
        return nullptr;
    }

    // ---- WHEN / AND clauses ----
    virtual std::any visitWhenClause(AMSParser::WhenClauseContext *ctx) override {
        return visitChildren(ctx);
    }

    // ---- Event Pattern (string or regex) ----
    virtual std::any visitEventPattern(AMSParser::EventPatternContext *ctx) override {
        if (ctx->STRING()) {
            std::string pattern = stripQuotes(ctx->STRING()->getText());
            bool isRegex = (ctx->getText().find("regex") != std::string::npos);
            if (isRegex) {
                std::cout << "  [WHEN] Regex pattern: " << pattern << std::endl;
            } else {
                std::cout << "  [WHEN] String pattern: \"" << pattern << "\"" << std::endl;
            }
        }
        return nullptr;
    }

    // ---- Time Window ----
    virtual std::any visitTimeWindow(AMSParser::TimeWindowContext *ctx) override {
        std::vector<antlr4::tree::TerminalNode*> ints = ctx->INT();
        std::string unit = ctx->timeUnit()->getText();

        if (ints.size() == 2) {
            std::cout << "  [TIME] Trigger when occurs " << ints[0]->getText()
                      << " times in " << ints[1]->getText() << " " << unit << std::endl;
        } else if (ints.size() == 1) {
            std::cout << "  [TIME] Window: " << ints[0]->getText() << " " << unit << std::endl;
        }
        return nullptr;
    }

    // ---- IF Clause ----
    virtual std::any visitIfClause(AMSParser::IfClauseContext *ctx) override {
        std::cout << "  [IF] Evaluating condition:" << std::endl;
        return visitChildren(ctx);
    }

    // ---- Condition (field comparison with AND/OR/NOT) ----
    virtual std::any visitCondition(AMSParser::ConditionContext *ctx) override {
        // Simple comparison: ID compOp value
        if (ctx->ID()) {
            std::string field = ctx->ID()->getText();
            std::string op = ctx->compOp()->getText();
            std::string val = ctx->value()->getText();
            if (val.length() >= 2 && val.front() == '"' && val.back() == '"') {
                val = val.substr(1, val.length() - 2);
            }
            std::cout << "    " << field << " " << op << " " << val << std::endl;
        }
        // Logical AND
        else if (ctx->children.size() >= 3) {
            for (size_t i = 0; i < ctx->children.size(); i++) {
                std::string text = ctx->children[i]->getText();
                if (text == "AND") {
                    std::cout << "    AND" << std::endl;
                } else if (text == "OR") {
                    std::cout << "    OR" << std::endl;
                } else if (text == "NOT") {
                    std::cout << "    NOT" << std::endl;
                }
            }
            // Visit sub-conditions
            for (auto subCond : ctx->condition()) {
                visit(subCond);
            }
        }
        // NOT condition
        else if (ctx->getText().find("NOT") == 0) {
            std::cout << "    NOT" << std::endl;
            visit(ctx->condition(0));
        }
        return nullptr;
    }

    // ---- DO Block ----
    virtual std::any visitDoBlock(AMSParser::DoBlockContext *ctx) override {
        std::cout << "  [DO] Executing actions:" << std::endl;
        return visitChildren(ctx);
    }

    // ---- Actions ----
    virtual std::any visitAction(AMSParser::ActionContext *ctx) override {
        std::string arg = stripQuotes(ctx->STRING()->getText());
        std::string text = ctx->getText();

        if (text.find("SEND_EMAIL") != std::string::npos) {
            std::cout << "    -> SEND_EMAIL to: " << arg << std::endl;
        } else if (text.find("BLOCK_IP") != std::string::npos) {
            std::cout << "    -> BLOCK_IP: " << arg << std::endl;
        } else if (text.find("ALERT") != std::string::npos) {
            std::cout << "    -> ALERT: " << arg << std::endl;
        } else if (text.find("LOG") != std::string::npos) {
            std::cout << "    -> LOG to file: " << arg << std::endl;
        } else if (text.find("CALL_API") != std::string::npos) {
            std::cout << "    -> CALL_API: " << arg << std::endl;
        } else if (text.find("EXECUTE_SCRIPT") != std::string::npos) {
            std::cout << "    -> EXECUTE_SCRIPT: " << arg << std::endl;
        } else if (text.find("CONSOLE") != std::string::npos) {
            std::cout << "    -> CONSOLE: " << arg << std::endl;
        }
        return nullptr;
    }

private:
    // Variable storage
    std::map<std::string, std::string> variables;

    // Helper: Remove surrounding quotes
    std::string stripQuotes(const std::string& s) {
        if (s.length() >= 2 && s.front() == '"' && s.back() == '"') {
            return s.substr(1, s.length() - 2);
        }
        return s;
    }
};