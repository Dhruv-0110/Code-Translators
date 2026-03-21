#pragma once
#include "AMSBaseVisitor.h"
#include <fstream>
#include <string>
#include <vector>

// ============================================================
// CodeGen : Compiled Execution Mode (ams build)
// Walks the AST and generates a standalone C++ monitoring program
// ============================================================
class CodeGen : public AMSBaseVisitor {
public:
    CodeGen(const std::string& outputFile) : out(outputFile) {}

    virtual std::any visitProgram(AMSParser::ProgramContext *ctx) override;
    virtual std::any visitMonitorStmt(AMSParser::MonitorStmtContext *ctx) override;
    virtual std::any visitWatchStmt(AMSParser::WatchStmtContext *ctx) override;
    virtual std::any visitSetStmt(AMSParser::SetStmtContext *ctx) override;
    virtual std::any visitRuleBlock(AMSParser::RuleBlockContext *ctx) override;
    virtual std::any visitSeverity(AMSParser::SeverityContext *ctx) override;
    virtual std::any visitWhenClause(AMSParser::WhenClauseContext *ctx) override;
    virtual std::any visitEventPattern(AMSParser::EventPatternContext *ctx) override;
    virtual std::any visitTimeWindow(AMSParser::TimeWindowContext *ctx) override;
    virtual std::any visitIfClause(AMSParser::IfClauseContext *ctx) override;
    virtual std::any visitCondition(AMSParser::ConditionContext *ctx) override;
    virtual std::any visitDoBlock(AMSParser::DoBlockContext *ctx) override;
    virtual std::any visitAction(AMSParser::ActionContext *ctx) override;

    void close() { if (out.is_open()) out.close(); }

private:
    std::ofstream out;

    std::string stripQuotes(const std::string& s) {
        if (s.length() >= 2 && s.front() == '"' && s.back() == '"') {
            return s.substr(1, s.length() - 2);
        }
        return s;
    }
};