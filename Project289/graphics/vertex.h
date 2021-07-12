#pragma once

#include <DirectXMath.h>

struct Vertex {
	Vertex() : pos(0.0f, 0.0f, 0.0f), color(0.0f, 0.0f, 0.0f, 1.0f), uv(0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), tg() {}
	Vertex(float x, float y, float z, float r, float g, float b, float u, float v, float nx, float ny, float nz) : pos(x, y, z), color(r, g, b, 1.0f), uv(u, v), normal(nx, ny, nz), tg() {}
	Vertex(const DirectX::XMFLOAT3& pos) : pos(pos), normal(), uv(), color(), tg() {}
	Vertex(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& n) : pos(pos), normal(n), uv(), color(), tg() {}
	Vertex(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& n, const DirectX::XMFLOAT2& t) : pos(pos), normal(n), uv(t), color(), tg() {}
	Vertex(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& n, const DirectX::XMFLOAT2& t, const DirectX::XMFLOAT3& tg) : pos(pos), normal(n), uv(t), color(), tg(tg) {}
	Vertex(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& n, const DirectX::XMFLOAT2& t, const DirectX::XMFLOAT3& tg, const DirectX::XMFLOAT4& color) : pos(pos), normal(n), uv(t), color(color), tg(tg) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tg;
};