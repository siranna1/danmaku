/*
J4ゲームプログラミング
シューティングゲームサンプル

操作方法
移動：方向キー
自機ショット：Zキー

画像素材
HamCorossam
http://homepage2.nifty.com/hamcorossam/

効果音素材
ザ・マッチメイカァズ
http://osabisi.sakura.ne.jp/m2/


ドット絵ツール
EDGE
http://takabosoft.com/edge
*/

#include "DxLib.h"
#define _USE_MATH_DEFINES
#include <math.h>

//ウィンドウサイズ
#define MIN_X 32
#define MIN_Y 16
#define MAX_X 416
#define MAX_Y 464

//画面の中心座標
#define CENTER_X ((MIN_X + MAX_X)/2)
#define CENTER_Y ((MIN_Y + MAX_Y)/2)

//角度の計算(°→rad)
#define OMEGA( t ) (t * M_PI / 180)

//敵の最大数
#define MAX_ENEMY 50

//画面内に最大1000発の弾
#define MAX_BULLET 2000

//自機ショットの最大数
#define MAX_PLAYER_SHOT 100

//弾の種類
#define NORMAL 1//円
#define LASER 2//レーザー

//敵の行動パターン
#define STOP 0//停止
#define STRAIGHT 1//直進
#define CIRCLE 2//円運動

//最大エフェクト数
#define MAX_EFFECT MAX_BULLET

//色
#define WHITE GetColor(255,255,255)//白

#define LEVEL_UP_SCORE 300 //敵1機で100点、1000なら10機でレベルアップ

#define BORN1 20 // 敵出現頻度
#define BORN2 1000 // ボス出現頻度

int t;//時間

int enemy_img;//敵の画像
int boss_img;
int bullet_img1,bullet_img2;//弾の画像
int shot_img;//自機ショットの画像
int board_img;//枠の画像
int back_img;//背景の画像
int effect_img[17];//エフェクトの画像
int score;//スコア

int shot_snd;//自機ショット発射音
int bullet_snd;//敵弾発射音
int bom_snd1;//爆発音1
int bom_snd2;//爆発音2
int up_snd;//パワーアップ音

//自機
struct Player
{
	int x;//座標           
	int y;
	int img;//画像
	int hp;//残機
	double range;//当たり判定
	bool isDamage;//被弾中ならtrue、被弾中でないならfalse
};

struct Player player;

//自機ショット
struct PlayerShot
{
	double x;//座標
	double y;
	double angle;//角度
	double speed;//速度
	double range;//当たり判定
	int img;//画像
	int power;//ショットの威力
	bool isExist;//存在したらtrue、いなかったらfalse
};

struct PlayerShot shot[MAX_PLAYER_SHOT];

//敵
struct Enemy
{
	double x;//座標
	double y;
	double speed;//速度
	double angle;//角度
	double range;//当たり判定
	int img;//画像
	int hp;//敵の体力
	int action;//敵の行動
	bool isExist;//存在したらtrue、いなかったらfalse
	bool isBoss;//ボスならtrue

};

struct Enemy enemy[MAX_ENEMY];
bool isBossExist = false;


//個々の弾
struct Bullet
{

	double x;//座標
	double y;
	double speed;//速度
	double angle;//角度
	double range ;//当たり判定
	int img;//画像
	bool isExist;//存在したらtrue、いなかったらfalse


};

struct Bullet bullet[MAX_BULLET];//MAX_BULLET個のBullet

//エフェクト
struct Effect
{
	int x;//座標
	int y;
	int img[20];//画像
	int max_img;//画像の最大数
	int t;//経過時間
	bool isExist;//存在したらtrue、いなかったらfalse
};

struct Effect effect[MAX_EFFECT];

void initEnemy(int i)
{
	enemy[i].x = 0;
	enemy[i].y = 0;
	enemy[i].range = 0;
	enemy[i].hp = 0;
	enemy[i].isExist = false;
	enemy[i].isBoss = false;
}
//初期化
void Init()
{
	int i;

	t=0;//時間初期化

	score=0;//スコア初期化

	//自機を適当な座標へ
	player.x = CENTER_X;
	player.y = CENTER_Y * 1.5 ;

	player.hp = 5;//残機5からスタート

	player.range = 3;//当たり判定

	player.isDamage = false;

	//敵の初期化
	for(i = 0; i < MAX_ENEMY; i++)
	{
		initEnemy(i);
	}

	//弾を全て初期化
	//エフェクト初期化
	for(i = 0; i < MAX_BULLET; i++)
	{
		bullet[i].isExist = false;
		bullet[i].x = 0;
		bullet[i].y = 0;
		bullet[i].speed = 0;
		bullet[i].angle = 0;
		bullet[i].range = 0;

		effect[i].isExist = false;
		effect[i].x = 0;
		effect[i].y = 0;
		effect[i].max_img = 0;
	}

}

//画像・音ファイルを読み込む関数
void LoadData()
{
	//画像読み込み
	player.img = LoadGraph("player.png");
	enemy_img = LoadGraph("smallenemy.png");
	boss_img = LoadGraph("boss.png");
	bullet_img1 = LoadGraph("bullet1.png");
	bullet_img2 = LoadGraph("bullet2.png");
	shot_img = LoadGraph("shot.png");
	board_img = LoadGraph("board.png");
	back_img = LoadGraph("back.png");
	LoadDivGraph("effect.png",17,8,3,64,64,effect_img);

	//音読み込み
	shot_snd = LoadSoundMem("push07.wav");
	bullet_snd = LoadSoundMem("close09.wav");
	bom_snd1 = LoadSoundMem("bom01.wav");
	bom_snd2 = LoadSoundMem("bom10.wav");
	up_snd = LoadSoundMem("power00.wav");

	// 音量の設定
	ChangeVolumeSoundMem( 255*0.1, shot_snd ) ;
	ChangeVolumeSoundMem( 255*0.5, bullet_snd ) ;

}

//エフェクト生成関数
void MakeEffect(int x, int y, int max)
{
	int i,j; 

	//使用中のエフェクトを調べる
	for( i = 0; i < MAX_EFFECT; i++)
	{
		if( !effect[i].isExist )
			break;
	}

	if ( i == MAX_EFFECT )//一つも空いてない
		return;

	effect[i].isExist = true;
	effect[i].x = x;
	effect[i].y = y;
	effect[i].t = 0;
	effect[i].max_img = max;

	for( j = 0; j < max; j++)
	{
		effect[i].img[j] = effect_img[j];
	}

}

//自機ショットの生成
void MakeShot(double speed, double angle, int power, double range)
{
	int i;

	//発射中のショットを調べる
	for(i = 0; i < MAX_PLAYER_SHOT; i++)
	{
		if( !shot[i].isExist)
			break;
	}
	if( i == MAX_PLAYER_SHOT)
		return;

	shot[i].isExist = true;

	shot[i].x = player.x;
	shot[i].y = player.y;

	shot[i].speed = speed;
	shot[i].angle = angle;
	shot[i].power = power;
	shot[i].range = range;

	shot[i].img = shot_img;

	PlaySoundMem( shot_snd , DX_PLAYTYPE_BACK ) ;//発射音

}

//多方向に自機ショットを発射
void MakeWayShot(double speed, int power, double range, int way, double wide_angle, double main_angle)
{
	int i; 

	double w_angle;
	
	if (way == 1)
	{
		MakeShot(speed, main_angle, power, range);
		return;
	}
	
	for( i = 0; i < way; i++)
	{
		if( wide_angle == OMEGA(360))
			w_angle = main_angle + i * wide_angle / way;//発射角度
		else
			w_angle = main_angle + i * wide_angle / ( way - 1 ) - wide_angle / 2;//発射角度

		MakeShot(speed,w_angle,power,range);
	}
}

//自機の移動
void ActionPlayer()
{
	const int fire = 4;
	const int speed = 4;

	double s_speed = 8;
	double s_angle = OMEGA( -90 );
	double range = 16;

	int power = 1;

	int way = 5;


	if( player.isDamage )//被弾中は動かさない
		return;
	//方向キーで移動
	if( CheckHitKey(KEY_INPUT_LEFT) )
		player.x -= speed;
	if( CheckHitKey(KEY_INPUT_RIGHT) )
		player.x += speed;
	if( CheckHitKey(KEY_INPUT_UP) )
		player.y -= speed;
	if( CheckHitKey(KEY_INPUT_DOWN) )
		player.y += speed;

	//ショットを撃つ
	if( CheckHitKey(KEY_INPUT_Z)  && t % fire == 0 )
	{
		//敵を10倒すごとに+1方向(最大6方向)
		way = score > LEVEL_UP_SCORE * 5 ? way : score / LEVEL_UP_SCORE + 1;
		MakeWayShot(s_speed,power,range,way,OMEGA( (way - 1) * 20 ),s_angle);
	}

	//移動を制限
	if( player.x < MIN_X )
		player.x = MIN_X;
	if( player.x > MAX_X )
		player.x = MAX_X;
	if( player.y < MIN_Y )
		player.y = MIN_Y;
	if( player.y > MAX_Y)
		player.y = MAX_Y;
}

//自機ショットの移動
void MoveShot()
{
	int i;

	double x,y,angle,speed;

	//発射中のショットを調べる
	for(i = 0; i < MAX_PLAYER_SHOT; i++)
	{
		if( !shot[i].isExist )
			continue;

		x = shot[i].x;
		y = shot[i].y;

		speed = shot[i].speed;
		angle = shot[i].angle;

		x += speed * cos( angle );
		y += speed * sin( angle );

		//弾が画面外に出た場合、弾を消す
		if( x < MIN_X || x > MAX_X || y < MIN_Y || y > MAX_Y)
			shot[i].isExist = false;

		shot[i].x = x;
		shot[i].y = y;

	}
}

//自機ショットの当たり判定処理
void JudgeShot()
{
	int i,j;
	double x,y;


	//存在する敵を調べる
	for(i = 0; i < MAX_ENEMY; i++)
	{

		if( !enemy[i].isExist )
			continue;

		//発射中のショットを全て調べる
		for(j = 0; j < MAX_PLAYER_SHOT; j++)
		{
			if( !shot[j].isExist )
				continue;

			x = shot[j].x - enemy[i].x;
			y = shot[j].y - enemy[i].y;

			//敵に自機ショットが当たれば
			if( hypot (x,y) < enemy[i].range + shot[j].range )
			{
				shot[j].isExist = false;

				enemy[i].hp -= shot[j].power;//敵の体力を減らす

				//敵の体力が無くなったら敵を消す
				if(enemy[i].hp < 0)
				{
					MakeEffect( enemy[i].x, enemy[i].y, 17);//爆発エフェクト

					PlaySoundMem( bom_snd1 , DX_PLAYTYPE_BACK ) ;//爆発音
					if (enemy[i].isBoss)
					{
						score += 2000;//スコア加算
						isBossExist = false;
					}
					else
						score += 100;//スコア加算
					initEnemy(i);
					if( score % LEVEL_UP_SCORE == 0 && score > 0 && score < LEVEL_UP_SCORE * 6)
						PlaySoundMem( up_snd , DX_PLAYTYPE_BACK ) ;//パワーアップ音
				}
			}

		}
	}
}

//自機に関する表示
void DrawPlayer()
{
	int i;

	//自機の表示
	DrawRotaGraphF( (float)player.x, (float)player.y, 1.0, 0, player.img, TRUE ) ;

	//自機ショットの表示
	for(i = 0; i < MAX_PLAYER_SHOT; i++)
	{
		if( shot[i].isExist )
			DrawRotaGraphF( (float)shot[i].x, (float)shot[i].y, 1.0, shot[i].angle, shot[i].img, TRUE ) ;
	}
}

//自機狙いの角度
double TargetAnglePlayer(double x, double y)
{	
	return atan2( player.y - y, player.x - x);	
}

//弾幕の生成

//方向弾(x,y:発射地点, speed:速度, angle:角度)
void MakeBullet(double x, double y, double speed, double angle, double range, int img)
{
	int i; 

	for( i = 0; i < MAX_BULLET; i++)
	{
		//bullet[i]が使用されていなければパラメータ設定へ
		if( !bullet[i].isExist )
			break;
	}
	if ( i == MAX_BULLET )//一つも空いてない
		return;

	bullet[i].isExist = true;

	//発射地点の座標
	bullet[i].x = x;
	bullet[i].y = y;

	bullet[i].angle = angle;//発射角度
	bullet[i].speed = speed;//速度

	bullet[i].range = range;

	bullet[i].img = img;//画像の代入

	PlaySoundMem( bullet_snd , DX_PLAYTYPE_BACK ) ;//発射音

}

//way弾(way:何方向に打つか, angle:扇形の角度, main_angle:扇形がどの方向を向くか)
void MakeWayBullet(double x, double y, double speed, int way, double wide_angle ,double main_angle, double range,int img)
{
	int i; 

	double w_angle;

	for( i = 0; i < way; i++)
	{
		if( wide_angle == OMEGA(360))
			w_angle = main_angle + i * wide_angle / way;//発射角度
		else
			w_angle = main_angle + i * wide_angle / ( way - 1 ) - wide_angle / 2;//発射角度

		MakeBullet(x,y,speed,w_angle,range,img);
	}

}

//弾の移動
void MoveBullet()
{
	double x, y;
	double angle;
	int i;

	//発射中の弾を全て調べる
	for(i = 0; i < MAX_BULLET; i++)
	{
		if( !bullet[i].isExist )
			continue;

		x = bullet[i].x;
		y = bullet[i].y;

		angle = bullet[i].angle;

		//角度angle方向にspeed分進める
		x += bullet[i].speed * cos( angle );
		y += bullet[i].speed * sin( angle );

		//弾が画面外に出た場合、弾を消す
		if( x < MIN_X || x > MAX_X || y < MIN_Y || y > MAX_Y)
			bullet[i].isExist = false;

		bullet[i].x = x;
		bullet[i].y = y;
	}

}

void JudgeBullet()
{
	int i;
	double x,y;

	//発射中の弾を全て調べる
	for(i = 0; i < MAX_BULLET; i++)
	{
		if( !bullet[i].isExist )
			continue;

		x = bullet[i].x - player.x;
		y = bullet[i].y - player.y;

		//自機が被弾中でない、当たり判定が接触したら
		if( hypot (x,y) < player.range + bullet[i].range && !player.isDamage)
		{
			MakeEffect(player.x, player.y, 17);
			bullet[i].isExist = false;
			player.isDamage = true;

			player.hp--;

			PlaySoundMem( bom_snd2, DX_PLAYTYPE_BACK ) ;//爆発音
		}
	}
}

//敵弾を全て消す
void EraseBullet()
{
	int i;

	for(i = 0; i < MAX_BULLET; i++)
		bullet[i].isExist = false;
}

//敵の出現
void MakeEnemy()
{
	int i;
	//ザコ敵 下側ランダム方向に直進
	if( t % BORN1 == 0 && !isBossExist )
	{
		for(i = 0;i < MAX_ENEMY; i++)
		{
			if( !enemy[i].isExist )
				break;
		}

		if( i == MAX_ENEMY )
			return ;

		enemy[i].isExist = true;

		//一番上のランダムな位置に出現
		enemy[i].x = GetRand(MAX_X - MIN_X) + MIN_X;
		enemy[i].y = MIN_Y;

		enemy[i].hp = 1;

		enemy[i].angle = OMEGA( GetRand(180) );//角度は0°～180°
		enemy[i].speed = GetRand(6) + 2;//速度は2～8

		enemy[i].range = 15;//当たり判定の大きさ

		enemy[i].action = STRAIGHT;//決まった角度に直進

		enemy[i].img = enemy_img;
	}

	//大型敵 座標固定
	if( t % BORN2 == 0 && t > 0 && !isBossExist )
	{
		for(i = 0;i < MAX_ENEMY; i++)
		{
			if( !enemy[i].isExist )
				break;
		}

		if( i == MAX_ENEMY )
			return ;

		isBossExist = true;
		enemy[i].isExist = true;
		enemy[i].isBoss = true;

		enemy[i].x = CENTER_X;
		enemy[i].y = MAX_Y / 4;

		enemy[i].hp = 200;

		enemy[i].angle = OMEGA( 90 );//下向き
		enemy[i].speed = 0;

		enemy[i].range = 130;

		enemy[i].action = STOP;//座標固定

		enemy[i].img = boss_img;
	}
}


//敵の行動
void ActionEnemy()
{
	int i;

	int fire;

	double speed;
	//	double angle;

	int way;
	//	double main_angle;

	double range;

	int x,y;

	for(i = 0; i < MAX_ENEMY; i++){

		if( !enemy[i].isExist )//敵がいれば次へ
			continue;

		switch(enemy[i].action)
		{
		case STOP://何もしない
			break;

		case STRAIGHT://決まった方向に直進

			enemy[i].x += enemy[i].speed * cos( enemy[i].angle );
			enemy[i].y += enemy[i].speed * sin( enemy[i].angle );

			break;

		}

		x = enemy[i].x;
		y = enemy[i].y;

		//弾が画面外に出た場合、弾を消す
		if( x < MIN_X || x > MAX_X || y < MIN_Y || y > MAX_Y)
			enemy[i].isExist = false;

		switch(enemy[i].action)
		{
		case STOP:
			fire = 40;
			x = enemy[i].x;
			y = enemy[i].y;
			speed = 2.0;
			way = 5;
			range = 8;
			//fire回のループごとに弾を発射
			if( t % fire == 0 )//3種類の弾幕
			{
				MakeWayBullet(x+140,y,1.5,3,OMEGA( 60 ),OMEGA(90)+OMEGA(30)*sin(OMEGA(t)),3,bullet_img2);
				MakeWayBullet(x-140,y,1.5,3,OMEGA( 60 ),OMEGA(90)+OMEGA(30)*sin(OMEGA(t)),3,bullet_img2);
				MakeWayBullet(x,y,speed,way,OMEGA(5),TargetAnglePlayer(x,y),range,bullet_img1);
			}
			break;

		case STRAIGHT:
			const int LEVEL_MAX_SCORE = 100000;
			if( score > LEVEL_MAX_SCORE )
				fire = 20;
			else
				fire = (LEVEL_MAX_SCORE - score) * 80 / LEVEL_MAX_SCORE + 20;
			x = enemy[i].x;
			y = enemy[i].y;
			speed = 2.0;
			way = 3;
			range = 8;
			//fire回のループごとに弾を発射
			if( t % fire == 0 )
			{
				switch(GetRand(1))//どちらかの弾幕を発射
				{
				case 0:
					MakeWayBullet(x,y,speed,way,OMEGA(30),TargetAnglePlayer(x,y),range,bullet_img1);
					break;

				case 1:
					MakeWayBullet(x,y,speed,2*way,OMEGA(360),OMEGA( GetRand(360) ),range,bullet_img1);
					break;
				}
			}
			break;

		}
	}
}

//弾の表示
void DrawBullet()
{
	double x,y,angle;
	int img;

	int i;

	//発射中の弾を全て調べる
	for(i = 0; i < MAX_BULLET; i++)
	{
		if( !bullet[i].isExist )
			continue;

		x = bullet[i].x ;
		y = bullet[i].y ;

		angle = bullet[i].angle ;

		img = bullet[i].img ;

		//弾の表示
		DrawRotaGraphF( (float)x, (float)y, 1.0, angle, img, TRUE ) ;

	}
}

//敵の表示
void DrawEnemy()
{
	int i;

	for( i = 0; i < MAX_ENEMY; i++ )
	{
		if( !enemy[i].isExist )//敵がいれば次へ
			continue;

		DrawRotaGraph( enemy[i].x, enemy[i].y, 1.0, enemy[i].angle, enemy[i].img, TRUE ) ;

	}

}

//エフェクトの表示
void DrawEffect()
{
	int i;
	int x,y;

	int ft;

	//発射中のエフェクトを全て調べる
	for(i = 0; i < MAX_EFFECT; i++)
	{
		if( !effect[i].isExist )
			continue;

		x = effect[i].x ;
		y = effect[i].y ;
		ft = effect[i].t ;

		DrawRotaGraphF( (float)x, (float)y, 1.0, 0, effect[i].img[ft], TRUE ) ;

		effect[i].t++ ;//エフェクトの時間を進める

		//全部表示し終わったら消す
		if(effect[i].t >= effect[i].max_img)
		{
			effect[i].isExist = false;

			if(effect[i].x == player.x && effect[i].y == player.y)//自機被弾の場合
			{
				EraseBullet();//敵弾をすべて消す
				player.isDamage = false;//被弾中ではなくなる
			}
		}
	}
}

//背景の表示

void DrawBack()
{
	int time;

	time = 8 * ( t % ( ( MAX_Y - MIN_Y ) / 8 ) );

	//背景スクロール表示

	DrawGraph( MIN_X, MIN_Y + time, back_img, false );

	if( time > 0 )
		DrawGraph( MIN_X, 2 * MIN_Y + time - MAX_Y, back_img, false );
}

//スコア等の表示
void DrawSystem()
{
	DrawGraph( 0, 0, board_img,true);

	DrawFormatString(MAX_X+50,MIN_Y+30,WHITE,"PLAYER：%d",player.hp);
	DrawFormatString(MAX_X+50,MIN_Y,WHITE,"SCORE：%d",score);
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow )
{
	// タイトルを 変更
	SetMainWindowText( "シューティングサンプル" ) ;
	// ウインドウモードに変更
	ChangeWindowMode( TRUE ) ;
	// ＤＸライブラリ初期化処理
	if( DxLib_Init() == -1 )		// ＤＸライブラリ初期化処理
		return -1 ;			// エラーが起きたら直ちに終了

	// 描画先画面を裏にする
	SetDrawScreen(DX_SCREEN_BACK);

	while(1)
	{	
		LoadData();
		Init();

		DrawString(0,330,"Press Space Bar",WHITE);//スペースキーを押して開始
		ScreenFlip();
		while(!CheckHitKey(KEY_INPUT_SPACE))
		{
			if ( ProcessMessage() == -1 )
			{
				DxLib_End();// ＤＸライブラリ使用の終了処理
				return 0;
			}		//メインループ Escキーで終了
		}
		while( !CheckHitKey(KEY_INPUT_ESCAPE) &&  player.hp > 0)
		{
			if ( ProcessMessage() == -1 )
			{
				DxLib_End();// ＤＸライブラリ使用の終了処理
				return 0;
			}

			ClearDrawScreen();

			//自機の移動
			ActionPlayer();

			//自機ショットの移動
			MoveShot();

			//敵の発生
			MakeEnemy();

			//敵の行動
			ActionEnemy();

			//弾の移動
			MoveBullet();

			//自機ショットの当たり判定処理
			JudgeShot();

			//敵弾の当たり判定処理
			JudgeBullet();

			//背景の表示
			DrawBack();

			//敵の表示
			DrawEnemy();

			//自機の表示
			DrawPlayer();

			//エフェクト表示
			DrawEffect();

			//弾の表示
			DrawBullet();

			DrawSystem();

			t++;//時間を進める

			// 裏画面の内容を表画面に反映
			ScreenFlip();
		}

		DrawString(0,300,"GAME OVER",WHITE);

		ScreenFlip();
	}
	DxLib_End();				// ＤＸライブラリ使用の終了処理
	return 0;
}
