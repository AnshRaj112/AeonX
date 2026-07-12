#include "myc/lexer/token_factory.hpp"
#include "myc/lexer/token_stream.hpp"

#include <algorithm>
#include <stdexcept>

namespace myc::lexer {

namespace {

[[nodiscard]] const Token& SentinelToken() {
    static const source::SourceLocation kLocation;
    static const source::SourceSpan kSpan;
    static const Token kEOF =
        TokenFactory::MakeEOF(kLocation, kSpan);
    return kEOF;
}

}  // namespace

TokenStream::TokenStream(std::vector<Token> tokens)
    : state_(std::make_shared<SharedState>(std::move(tokens))) {}

TokenStream TokenStream::FromVector(std::vector<Token> tokens) {
    return TokenStream(std::move(tokens));
}

TokenStream TokenStream::Share() const noexcept { return TokenStream(state_, position_); }

std::size_t TokenStream::Size() const noexcept {
    return state_ ? state_->tokens.size() : 0;
}

bool TokenStream::Empty() const noexcept { return Size() == 0; }

bool TokenStream::IsAtEnd() const noexcept {
    return state_ == nullptr || position_ >= state_->tokens.size();
}

std::size_t TokenStream::GetPosition() const noexcept { return position_; }

const Token& TokenStream::Peek(std::size_t offset) const {
    if (!state_) {
        return SentinelToken();
    }
    const std::size_t index = position_ + offset;
    if (index >= state_->tokens.size()) {
        return SentinelToken();
    }
    return state_->tokens[index];
}

const Token& TokenStream::Lookahead(std::size_t count) const { return Peek(count); }

const Token& TokenStream::Consume() {
    if (!state_ || position_ >= state_->tokens.size()) {
        return SentinelToken();
    }
    return state_->tokens[position_++];
}

const Token& TokenStream::Previous() const {
    if (!state_ || position_ == 0) {
        return SentinelToken();
    }
    return state_->tokens[position_ - 1];
}

void TokenStream::Reset() noexcept { position_ = 0; }

void TokenStream::ResetTo(std::size_t position) {
    if (!state_) {
        position_ = 0;
        return;
    }
    position_ = std::min(position, state_->tokens.size());
}

std::size_t TokenStream::SaveCheckpoint() const noexcept { return position_; }

void TokenStream::RestoreCheckpoint(std::size_t checkpoint) noexcept { ResetTo(checkpoint); }

const Token& TokenStream::operator[](std::size_t index) const {
    if (!state_) {
        throw std::out_of_range("token stream is empty");
    }
    return state_->tokens.at(index);
}

const std::vector<Token>& TokenStream::GetTokens() const noexcept {
    static const std::vector<Token> kEmpty;
    return state_ ? state_->tokens : kEmpty;
}

TokenIterator TokenStream::begin() const noexcept {
    return TokenIterator(state_ ? &state_->tokens : nullptr, 0);
}

TokenIterator TokenStream::end() const noexcept {
    return TokenIterator(state_ ? &state_->tokens : nullptr, Size());
}

TokenIterator TokenStream::cbegin() const noexcept { return begin(); }

TokenIterator TokenStream::cend() const noexcept { return end(); }

}  // namespace myc::lexer
