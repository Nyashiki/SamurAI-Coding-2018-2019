#ifndef GAME_H_
#define GAME_H_

#include "state.h"

/*
 * ゲームのメタ状態
 */
namespace Game {

extern int remain_time;  // 残り思考時間（ミリ秒）
extern int step_max;  // 最大のステップ数
extern int height, width;  // コースの距離と横幅
extern int sight_range;  // 視界の広さ

extern State* current_state;
extern Player players[2];

void Init();

// ゲーム初期化時の入力を受け取る
void InitializationInput();

// ステップごとの入力を受け取る
void StepInput();

// ステップごとの出力を吐き出す
void OutputAction();

}  // namespace Game


#endif  // GAME_H_
