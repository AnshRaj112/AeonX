#pragma once

#include "myc/lexer/token.hpp"

#include <cstddef>
#include <iterator>
#include <memory>
#include <vector>

namespace myc::lexer {

class TokenStream;

/// STL-style const iterator over a token sequence.
class TokenIterator {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Token;
    using difference_type = std::ptrdiff_t;
    using pointer = const Token*;
    using reference = const Token&;

    TokenIterator() = default;

    [[nodiscard]] reference operator*() const { return (*tokens_)[index_]; }
    [[nodiscard]] pointer operator->() const { return &(*tokens_)[index_]; }

    TokenIterator& operator++() {
        ++index_;
        return *this;
    }

    TokenIterator operator++(int) {
        TokenIterator copy = *this;
        ++(*this);
        return copy;
    }

    TokenIterator& operator--() {
        --index_;
        return *this;
    }

    TokenIterator operator--(int) {
        TokenIterator copy = *this;
        --(*this);
        return copy;
    }

    TokenIterator& operator+=(difference_type offset) {
        index_ += static_cast<std::size_t>(offset);
        return *this;
    }

    TokenIterator& operator-=(difference_type offset) {
        index_ -= static_cast<std::size_t>(offset);
        return *this;
    }

    [[nodiscard]] TokenIterator operator+(difference_type offset) const {
        TokenIterator copy = *this;
        copy += offset;
        return copy;
    }

    [[nodiscard]] TokenIterator operator-(difference_type offset) const {
        TokenIterator copy = *this;
        copy -= offset;
        return copy;
    }

    [[nodiscard]] difference_type operator-(const TokenIterator& other) const {
        return static_cast<difference_type>(index_) -
               static_cast<difference_type>(other.index_);
    }

    [[nodiscard]] reference operator[](difference_type offset) const {
        return (*tokens_)[index_ + static_cast<std::size_t>(offset)];
    }

    [[nodiscard]] auto operator<=>(const TokenIterator& other) const noexcept = default;

private:
    friend class TokenStream;

    TokenIterator(const std::vector<Token>* tokens, std::size_t index) noexcept
        : tokens_(tokens), index_(index) {}

    const std::vector<Token>* tokens_{nullptr};
    std::size_t index_{0};
};

/// Shared, immutable token sequence with navigable cursor state.
///
/// Multiple `TokenStream` instances can share the same underlying storage via
/// `Share()` or copy construction, enabling parallel compiler stages to read
/// tokens concurrently while maintaining independent cursor positions.
class TokenStream {
public:
    TokenStream() = default;
    explicit TokenStream(std::vector<Token> tokens);

    [[nodiscard]] static TokenStream FromVector(std::vector<Token> tokens);
    [[nodiscard]] TokenStream Share() const noexcept;

    [[nodiscard]] std::size_t Size() const noexcept;
    [[nodiscard]] bool Empty() const noexcept;
    [[nodiscard]] bool IsAtEnd() const noexcept;
    [[nodiscard]] std::size_t GetPosition() const noexcept;

    [[nodiscard]] const Token& Peek(std::size_t offset = 0) const;
    [[nodiscard]] const Token& Lookahead(std::size_t count) const;
    [[nodiscard]] const Token& Consume();
    [[nodiscard]] const Token& Previous() const;

    void Reset() noexcept;
    void ResetTo(std::size_t position);

    [[nodiscard]] std::size_t SaveCheckpoint() const noexcept;
    void RestoreCheckpoint(std::size_t checkpoint) noexcept;

    [[nodiscard]] const Token& operator[](std::size_t index) const;
    [[nodiscard]] const std::vector<Token>& GetTokens() const noexcept;

    [[nodiscard]] TokenIterator begin() const noexcept;
    [[nodiscard]] TokenIterator end() const noexcept;
    [[nodiscard]] TokenIterator cbegin() const noexcept;
    [[nodiscard]] TokenIterator cend() const noexcept;

private:
    struct SharedState {
        explicit SharedState(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

        const std::vector<Token> tokens;
    };

    TokenStream(std::shared_ptr<const SharedState> state, std::size_t position) noexcept
        : state_(std::move(state)), position_(position) {}

    std::shared_ptr<const SharedState> state_;
    std::size_t position_{0};
};

}  // namespace myc::lexer
