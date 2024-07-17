/*
 j2130 福頼雄倫

	球をはさむように二つの線が出るようになっています
	tanが２つの線を出すのにいい働きしてます
*/



#define _USE_MATH_DEFINES
#include "DxLib.h"
#include <math.h>



//ウィンドウサイズ
#define MIN_X 0
#define MIN_Y 0
#define MAX_X 640
#define MAX_Y 480

//画面の中心座標
#define CENTER_X ((MIN_X + MAX_X)/2)
#define CENTER_Y ((MIN_Y + MAX_Y)/2)

//時間tの最大値
#define MAX_T 3600

//角度の計算
#define OMEGA( t ) (t * M_PI / 180)

//色
#define WHITE GetColor( 255, 255, 255 )//画面表示する文字の色

int t;//時間
double x,y;//戦闘機の座標
double r;//半径
int fighter_img;//画像用変数

//軌道描画用の変数
double track_x[MAX_T],track_y[MAX_T];

void Init()
{
	x = CENTER_X;
	y = CENTER_Y;
	t = 0;
	r = 100;
}

//画像・音ファイルを読み込む関数
void LoadData()
{
	fighter_img = LoadGraph("fighter.png");
}

//戦闘機の移動
void MoveFighter()
{
	x = CENTER_X + cos(OMEGA(t))  * sin((double)3/7 * OMEGA(t)) * r + log(x * x) + tan(OMEGA(t));
	//x = r / 10 * (OMEGA(t) - cos(OMEGA(t))) +x*x*x + sin(OMEGA(t)) * sin(OMEGA(t) * x);
	y = CENTER_Y + sin(OMEGA(t)) * r  + log(t + 4) + cos(OMEGA(t) * OMEGA(t));
	
    //座標を記録
	track_x[t] = x;
	track_y[t] = y;

}

//戦闘機の描画
void DrawFighter()
{
	
	//座標表示
	DrawFormatString( 0, 0, WHITE, "座標　x：%.2f, y：%.2f", x, y );
	
	//戦闘機表示
	DrawGraph( (int)x, (int)y, fighter_img, TRUE ) ;
    //角度に合わせて画像も回転
	//DrawRotaGraph( (int)x, (int)y, 1.0f, OMEGA(t), fighter_img, TRUE ) ;
	//軌道表示
	for(int i=1; i < t; i++){
		DrawLine((int)track_x[i-1], (int)track_y[i-1], (int)track_x[i], (int)track_y[i], WHITE, false);
	}
	
}

//初期化をするかどうかチェック
//初期化するならtrue, しないならfalseを返す
bool isInit()
{
	if( t >= MAX_T )
		return true;
	return false;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine, int nCmdShow )
{
	// タイトルを 変更
	SetMainWindowText( "直線運動サンプル" ) ;
	// ウインドウモードに変更
	ChangeWindowMode( TRUE ) ;
	// ＤＸライブラリ初期化処理
	if( DxLib_Init() == -1 )		// ＤＸライブラリ初期化処理
		return -1 ;			// エラーが起きたら直ちに終了
	
	// 描画先画面を裏にする
	SetDrawScreen(DX_SCREEN_BACK);
	
	LoadData();
	Init();
	
	//メインループ Escキーで終了
	while(!ProcessMessage() && !CheckHitKey(KEY_INPUT_ESCAPE))
	{
		
		ClearDrawScreen();
        
        //戦闘機の移動
		MoveFighter();
		
        //戦闘機の表示
		DrawFighter();
		
		t++;//時間を進める
		
        //初期化判定
		if(isInit())
			Init();
			
		// 裏画面の内容を表画面に反映
		ScreenFlip();
	}
	return 0;
}
