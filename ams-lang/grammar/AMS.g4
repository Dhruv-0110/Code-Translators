grammar AMS;

// ===================== PARSER RULES =====================

program : statement+ EOF ;

statement
    : monitorStmt
    | watchStmt
    | setStmt
    | ruleBlock
    ;

// === MONITOR / WATCH Data Sources ===
monitorStmt : 'MONITOR' STRING ;
watchStmt   : 'WATCH' ID STRING ;

// === SET Variable Assignment ===
setStmt : 'SET' ID '=' value ;

// === RULE Block (Core Detection Unit) ===
ruleBlock
    : 'RULE' ID severity? whenClause+ ifClause? doBlock 'END'
    ;

// === SEVERITY Level (Rule Priority) ===
severity : 'SEVERITY' severityLevel ;
severityLevel : 'LOW' | 'MEDIUM' | 'HIGH' | 'CRITICAL' ;

// === WHEN Clause (Event Detection) ===
whenClause
    : 'WHEN' eventPattern timeWindow?
    | 'AND' eventPattern timeWindow?
    ;

eventPattern
    : STRING                        // Simple string pattern match
    | 'regex' '(' STRING ')'        // Regex pattern match
    ;

timeWindow
    : 'OCCURS' INT 'TIMES' 'IN' INT timeUnit
    | 'WITHIN' INT timeUnit
    | 'IN' INT timeUnit
    ;

timeUnit : 'SECONDS' | 'MINUTES' | 'HOURS' ;

// === IF Clause (Field-Based Conditions) ===
ifClause : 'IF' condition ;

condition
    : ID compOp value                       // Simple comparison
    | condition 'AND' condition             // Logical AND
    | condition 'OR' condition              // Logical OR
    | 'NOT' condition                       // Logical NOT
    | '(' condition ')'                     // Grouped condition
    ;

compOp : '==' | '!=' | '>' | '<' | '>=' | '<=' ;

value
    : STRING
    | INT
    | 'TRUE'
    | 'FALSE'
    ;

// === DO Block (Automated Actions) ===
doBlock : 'DO' action+ ;

action
    : 'SEND_EMAIL' '(' STRING ')'
    | 'ALERT' '(' STRING ')'
    | 'LOG' '(' STRING ')'
    | 'CALL_API' '(' STRING ')'
    | 'EXECUTE_SCRIPT' '(' STRING ')'
    | 'CONSOLE' '(' STRING ')'
    | 'BLOCK_IP' '(' STRING ')'
    ;

// ===================== LEXER RULES =====================

INT     : [0-9]+ ;
ID      : [a-zA-Z_][a-zA-Z0-9_]* ;
STRING  : '"' (~["\r\n])* '"' ;
WS      : [ \t\r\n]+ -> skip ;
COMMENT : '//' ~[\r\n]* -> skip ;