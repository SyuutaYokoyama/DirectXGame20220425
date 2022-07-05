#include "Skydome.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include <random>

void Skydome::Initialize(Model* model, uint32_t textureHandle) {
	//NULL�|�C���^�`�F�b�N
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	//������
	worldTransform_.Initialize();
	//�������W
	worldTransform_.translation_ = { 6.0f,0.0f,30.0f };
	//�e�N�X�`���ǂݍ���
	//textureHandle_ = TextureManager::Load("skydome.png");
}
void Skydome::Update() {}

void Skydome::Draw(ViewProjection &viewprojection) {
	model_->Draw(worldTransform_, viewprojection, textureHandle_);
}