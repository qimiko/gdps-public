#pragma once

#include <imgui.h>

inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) { return {a.x + b.x, a.y + b.y}; }
inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) { return {a.x - b.x, a.y - b.y}; }
inline ImVec2 operator/(ImVec2 const& a, float b) { return { a.x / b, a.y / b }; }
inline ImVec2 operator*(ImVec2 const& a, float b) { return { a.x * b, a.y * b }; }

constexpr ImU32 rgb(size_t value) {
	const auto r = value >> 16;
	const auto g = (value >> 8) & 0xFF;
	const auto b = value & 0xFF;
	return 0xFF000000 | (b << 16) | (g << 8) | r;
}

constexpr ImU32 rgba(size_t value) {
	const auto r = value >> 24;
	const auto g = (value >> 16) & 0xFF;
	const auto b = (value >> 8) & 0xFF;
	const auto a = value & 0xFF;
	return (a << 24) | (b << 16) | (g << 8) | r;
}
