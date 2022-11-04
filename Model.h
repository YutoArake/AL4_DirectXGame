#pragma once

#include <DirectXMath.h>
#include <d3dx12.h>

/// <summary>
/// 3Dモデル
/// </summary>
class Model {
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public: // サブクラス
	// 頂点データ構造体
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos; // xyz座標
		XMFLOAT3 normal; // 法線ベクトル
		XMFLOAT2 uv;  // uv座標
	};

	// 定数バッファ用データ構造体B1
	struct ConstBufferDataB1
	{
		XMFLOAT3 ambient;	// アンビエント係数
		float pad1;					// パディング
		XMFLOAT3 diffuse;		// ディフューズ係数
		float pad2;					// パディング
		XMFLOAT3 specular;	// スペキュラー係数
		float alpha;				// アルファ
	};

	// マテリアル
	struct Material {
		std::string name;			// マテリアル名
		XMFLOAT3 ambient;		// アンビエント影響度
		XMFLOAT3 diffuse;			// ディフーズ影響度
		XMFLOAT3 specular;		// スペキュラー影響度
		float alpha;					// アルファ
		std::string textureFilename;	// ファイル名
		// コンストラクタ
		Material() {
			ambient = { 0.3f, 0.3f, 0.3f };
			diffuse = { 0.0f, 0.0f, 0.0f };
			specular = { 0.0f, 0.0f, 0.0f };
			alpha = 1.0f;
		}
	};

public: // 静的メンバ関数
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(static ID3D12Device* device);

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device;
	// デスクリプタサイズ
	static UINT descriptorHandleIncrementSize;
	// テクスチャバッファ
	static ComPtr<ID3D12Resource> texbuff;
	// シェーダリソースビューのハンドル(CPU)
	static CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// シェーダリソースビューのハンドル(CPU)
	static CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	// デスクリプタヒープ
	static ComPtr<ID3D12DescriptorHeap> descHeap;

private:// 静的メンバ関数
	/// <summary>
	/// デスクリプタヒープの初期化
	/// </summary>
	static void InitializeDescriptorHeap();

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	/// <returns>成否</retunrs>
	static void LoadTexture(const std::string& directoryPath, const std::string& filename);

private: // メンバ関数
	/// <summary>
	/// モデル作成
	/// </summary>
	void CreateModel();

	/// <summary>
	/// マテリアル読み込み
	/// </summary>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

public: // メンバ関数
	/// <summary>
	/// 初期化処理処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(static ID3D12GraphicsCommandList* cmdList);

private: // メンバ変数
	// ComPtr<ID3D12Resource> constBuff; // 定数バッファ
	ComPtr<ID3D12Resource> constBuffB1; // 定数バッファ
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView;
	// 頂点データ配列
	// static VertexPosNormalUv vertices[vertexCount];
	std::vector<VertexPosNormalUv> vertices;
	// 頂点インデックス配列
	// static unsigned short indices[planeCount * 3];
	std::vector<unsigned short> indices;

	// マテリアル
	Material material;
};