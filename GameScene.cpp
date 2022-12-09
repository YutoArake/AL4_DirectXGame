﻿#include "GameScene.h"
#include "Collision.h"
#include <sstream>
#include <iomanip>
#include <cassert>

using namespace DirectX;

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete object3d;
	delete togeObj;
	delete model_;
	delete togeModel_;

	delete spriteBG;
	delete sprite1;
	delete sprite2;
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input)
{
	// nullptrチェック
	assert(dxCommon);
	assert(input);

	this->dxCommon = dxCommon;
	this->input = input;

	// デバッグテキスト用テクスチャ読み込み
	Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png");
	// デバッグテキスト初期化
	debugText.Initialize(debugTextTexNumber);

	// テクスチャ読み込み
	Sprite::LoadTexture(1, L"Resources/background.png");

	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });
	// モデル生成
	model_ = Model::CreateFromOBJ("toge");
	togeModel_ = Model::CreateFromOBJ("toge2");
	// 3Dオブジェクト生成
	object3d = Object3d::Create();
	togeObj = Object3d::Create();
	// 3Dオブジェクトにモデルを割り当てる
	object3d->SetModel(model_);
	togeObj->SetModel(togeModel_);

	// テクスチャ2番に読み込む
	Sprite::LoadTexture(2, L"Resources/texture.png");

	// 座標{ 0, 0 }に、テクスチャ2番のスプライトを生成
	sprite1 = Sprite::Create(2, { 0, 0 });
	// 座標{ 500, 500 }に、テクスチャ2番のスプライトを生成
	sprite2 = Sprite::Create(2, { 500, 500 }, { 1, 0, 0, 1 }, { 0, 0 }, false, true);

	// 球の初期値を設定
	sphere.center = XMVectorSet(0, 2, 0, 1);		// 中心点座標
	sphere.radius = 1.0f;	// 半径

	// 平面の初期値を設定
	plane.normal = XMVectorSet(0, 1, 0, 0);		// 法線ベクトル
	plane.distance = 0.0f;	// 原点( 0,0,0 )からの距離

	// 三角形の初期値を設定
	triangle.p0 = XMVectorSet(-1.0f, 0, -1.0f, 1);	// 左手前
	triangle.p1 = XMVectorSet(-1.0f, 0, +1.0f, 1);	// 左奥
	triangle.p2 = XMVectorSet(+1.0f, 0, -1.0f, 1);	// 右手前
	triangle.normal = XMVectorSet(0.0f, 1.0f, 0.0f, 0);	// 上向き

	// レイの初期値を設定
	ray.start = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);		// 原点よりやや上
	ray.dir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);		// 下向き
}

void GameScene::Update()
{
	// オブジェクト移動
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT))
	{
		// 現在の座標を取得
		XMFLOAT3 position = object3d->GetPosition();

		// 移動後の座標を計算
		if (input->PushKey(DIK_UP)) { position.y += 1.0f; }
		else if (input->PushKey(DIK_DOWN)) { position.y -= 1.0f; }
		if (input->PushKey(DIK_RIGHT)) { position.x += 1.0f; }
		else if (input->PushKey(DIK_LEFT)) { position.x -= 1.0f; }

		// 座標の変更を反映
		object3d->SetPosition(position);
	}

	// カメラ移動
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A))
	{
		if (input->PushKey(DIK_W)) { Object3d::CameraMoveVector({ 0.0f,+1.0f,0.0f }); }
		else if (input->PushKey(DIK_S)) { Object3d::CameraMoveVector({ 0.0f,-1.0f,0.0f }); }
		if (input->PushKey(DIK_D)) { Object3d::CameraMoveVector({ +1.0f,0.0f,0.0f }); }
		else if (input->PushKey(DIK_A)) { Object3d::CameraMoveVector({ -1.0f,0.0f,0.0f }); }
	}

	object3d->Update();
	togeObj->Update();

	// レイ操作
	{
		XMVECTOR moveZ = XMVectorSet(0, 0, 0.01f, 0);
		if (input->PushKey(DIK_T)) { ray.start += moveZ; }
		else if (input->PushKey(DIK_G)) { ray.start -= moveZ; }

		XMVECTOR moveX = XMVectorSet(0.01f, 0, 0, 0);
		if (input->PushKey(DIK_H)) { ray.start += moveX; }
		else if (input->PushKey(DIK_F)) { ray.start -= moveX; }
	}

	// stringstreamで変数の値を埋め込んで整形する
	std::ostringstream raystr;
	raystr << "ray.start:("
		<< std::fixed << std::setprecision(2)		// 小数点以下2桁まで
		<< ray.start.m128_f32[0] << ","		// x
		<< ray.start.m128_f32[1] << ","		// y
		<< ray.start.m128_f32[2] << ")";	// z

	debugText.Print(raystr.str(), 50, 180, 1.0f);

	// レイと平面の当たり判定
	XMVECTOR inter;
	float distance;
	bool hit = Collision::CheckRay2Triangle(ray, triangle, &distance, &inter);

	if (hit) {
		debugText.Print("HIT", 50, 200, 1.0f);
		// stringstreamをリセットし、交点座標を埋め込む
		raystr.str("");
		raystr.clear();
		raystr << "inter:("
			<< std::fixed << std::setprecision(2)
			<< inter.m128_f32[0] << ","
			<< inter.m128_f32[1] << ","
			<< inter.m128_f32[2] << ")";

		debugText.Print(raystr.str(), 50, 220, 1.0f);

		raystr.str("");
		raystr.clear();
		raystr << "distance:("
			<< std::fixed << std::setprecision(2)
			<< distance << ")";

		debugText.Print(raystr.str(), 50, 240, 1.0f);
	}

	// スプライト移動
	if (input->PushKey(DIK_SPACE)) {
		// 現在の座標を取得
		XMFLOAT2 position = sprite1->GetPosition();
		// 移動後の座標を計算
		position.x += 1.0f;
		// 座標の変更を反映
		sprite1->SetPosition(position);
	}
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	// 背景スプライト描画
	// spriteBG->Draw();

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(cmdList);

	// 3Dオブクジェクトの描画
	object3d->Draw();
	togeObj->Draw();

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// 描画
	// sprite1->Draw();
	// sprite2->Draw();

	// デバッグテキストの描画
	debugText.DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}
