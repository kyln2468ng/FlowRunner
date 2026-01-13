#pragma once
#include <d3d11.h>
#include <wincodec.h>
#include "string"

class Texture
{
public:
	Texture();
	~Texture();
	HRESULT Load(std::string fileName);
	void Release();
	ID3D11SamplerState* GetSampler() const{ return pSampler_; }
	ID3D11ShaderResourceView* GetSRV() const{ return pSRV_; }
private:
	ID3D11SamplerState* pSampler_;
	ID3D11ShaderResourceView* pSRV_;
};
