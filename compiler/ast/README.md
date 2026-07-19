# Myc AST Module

The AST module defines the abstract syntax tree hierarchy produced by the parser. Nodes carry source spans for diagnostics and are designed for visitor-based traversal by future compiler passes.

## Hierarchy

```
ASTNode
├── ProgramNode
├── DeclarationNode
│   ├── FunctionDeclarationNode
│   ├── ClassDeclarationNode
│   ├── StructDeclarationNode
│   ├── TraitDeclarationNode
│   ├── EnumDeclarationNode
│   ├── ImportDeclarationNode
│   ├── ModuleDeclarationNode
│   └── PackageDeclarationNode
├── StatementNode
│   ├── ExpressionStatementNode
│   ├── VariableDeclarationNode
│   ├── ReturnStatementNode
│   ├── IfStatementNode
│   ├── WhileStatementNode
│   ├── ForStatementNode
│   ├── BlockStatementNode
│   ├── FutureSwitchStatementNode
│   └── FutureAsyncStatementNode
├── ExpressionNode
│   ├── IdentifierExpressionNode
│   ├── LiteralExpressionNode
│   ├── CallExpressionNode
│   ├── MemberAccessExpressionNode
│   ├── UnaryExpressionNode
│   ├── BinaryExpressionNode
│   ├── ParenthesizedExpressionNode
│   ├── AssignmentExpressionNode
│   ├── FutureLambdaExpressionNode
│   └── FutureMatchExpressionNode
├── TypeNode
│   ├── PrimitiveTypeNode
│   ├── NamedTypeNode
│   └── FutureGenericTypeNode
└── LiteralNode
    ├── IntegerLiteralNode
    ├── FloatLiteralNode
    ├── BooleanLiteralNode
    ├── StringLiteralNode
    └── CharacterLiteralNode
```

## Ownership

All child nodes are owned via `std::unique_ptr`. Collections use `std::vector<std::unique_ptr<...>>`. No raw owning pointers are used.

## Visitor Design

`AstVisitor` and `AstConstVisitor` provide double-dispatch traversal:

```cpp
class MyPass : public myc::ast::AstConstVisitor {
    void VisitCallExpression(const CallExpressionNode& node) override {
        // inspect call sites
    }
};

MyPass pass;
program->Accept(pass);
```

Default visitor methods are no-ops, so passes override only the node kinds they need.

## Pretty Printer

`AstPrettyPrinter` is a debug-only `AstConstVisitor` that renders a human-readable tree:

```
CallExpression
  Callee
    MemberAccessExpression
      Receiver
        Identifier(io)
      Member
        println
  Arguments
    StringLiteral("Hello")
```

## Future Extensions

Placeholder nodes (`FutureModuleNode`, `FutureTraitNode`, `FutureGenericNode`, `FutureLambdaExpressionNode`, `FutureMatchExpressionNode`) reserve extension points for:

- Generic types and monomorphization
- Pattern matching
- Async/await syntax
- Module system refinements

## CMake

```cmake
target_link_libraries(your_target PRIVATE myc::ast)
```
