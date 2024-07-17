#include "DxLib.h"
#define _USE_MATH_DEFINES
#include <math.h>

//ウィンドウサイズ
#define MIN_X 0
#define MIN_Y 0
#define MAX_X 640
#define MAX_Y 480

//画面の中心座標
#define CENTER_X ((MIN_X + MAX_X)/2)
#define CENTER_Y ((MIN_Y + MAX_Y)/2)

//角度の計算(°→rad)
#define OMEGA( t ) (t * M_PI / 180)

//画面内に最大1000発の弾
#define MAX_BULLET 2000

//#define BLAST_FIG_NUM 16//爆発するアニメの画像数
//int BlastHandle[BLAST_FIG_NUM];//爆発画像ハンドル
int t;//時間
double dt = 0.01;
int bullet_img;//弾の画像

//自機
struct Player
{
	int x;                   
	int y;
	int img;
	double range;//当たり判定の半径
};

struct Player player;

//敵
struct Enemy
{
	int x;//座標
	int y;
	int img;//画像
	
};

struct Enemy enemy;

//個々の弾
struct Bullet
{
	double x;//座標
	double y;
	double speed;
	double angle;
	double range ;//当たり判定の半径
	bool isExist;//存在したらtrue、いなかったらfalse
	int img;//画像
	bool tracking;

	double accelerationX;
	double accelerationY;
	double velocityX;
	double velocityY;
	double time;
};

struct Bullet bullet[MAX_BULLET];//MAX_BULLET個のBullet


//初期化
void Init()
{
	int i;

	t=0;//時間初期化

	//自機を適当な座標へ
	player.x = CENTER_X;
	player.y = CENTER_Y * 3 / 2 ;

	player.range = 5;

	//敵を適当な座標へ
	enemy.x=CENTER_X;
	enemy.y=CENTER_Y/2;

	//弾を全て初期化
	for(i = 0; i < MAX_BULLET; i++)
	{
		bullet[i].isExist = false;
		bullet[i].x = 0;
		bullet[i].y = 0;
		bullet[i].speed = 0;
		bullet[i].angle = 0;
		bullet[i].range = 0;
		bullet[i].tracking = true;
		bullet[i].accelerationX = 0;
		bullet[i].accelerationY = 0;
		bullet[i].velocityX = 0;
		bullet[i].velocityY = 0;
		bullet[i].time = 2;
	}

}

//画像・音ファイルを読み込む関数
void LoadData()
{
	player.img = LoadGraph("player.png");
	enemy.img = LoadGraph("enemy.png");
	bullet_img = LoadGraph("bullet.png");
//	LoadDivGraph( "bomb0.png", BLAST_FIG_NUM, 8, 2, 768/8, 192/2, BlastHandle );//爆発画像登録 640x480bomb0.png
}

//自機の移動
void MovePlayer()
{
	const int speed = 4;

	if( CheckHitKey(KEY_INPUT_LEFT) )
		player.x -= speed;
	if( CheckHitKey(KEY_INPUT_RIGHT) )
		player.x += speed;
	if( CheckHitKey(KEY_INPUT_UP) )
		player.y -= speed;
	if( CheckHitKey(KEY_INPUT_DOWN) )
		player.y += speed;

	if( player.x < MIN_X )
		player.x = MIN_X;
	else if( player.x > MAX_X )
		player.x = MAX_X;
	if( player.y < MIN_Y )
		player.y = MIN_Y;
	else if( player.y > MAX_Y)
		player.y = MAX_Y;

}

//自機の表示
void DrawPlayer()
{
	DrawRotaGraph( player.x, player.y, 1.0, 90, player.img, TRUE ) ;
}

//自機狙いの角度
double TargetAnglePlayer(double x, double y)
{	
	return atan2(player.y - y, player.x - x);
}

//弾幕の生成

//方向弾(x,y:発射地点, speed:速度, angle:角度)
void MakeBullet(double x, double y, double speed, double angle, double range)
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
	bullet[i].img = bullet_img;//画像の代入
}

void MakeWayBullet(double x, double y, double speed, 
	int way, double wide_angle, double main_angle, double range)
{
	double w_angle;
	for (int i = 0; i < way; i++)
	{
		w_angle = main_angle + i * wide_angle/(way-1) - wide_angle / 2;
		MakeBullet(enemy.x, enemy.y, speed, w_angle, range);
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

void MoveBullet2()
{
	double x, y;
	double angle;
	int i;

	//発射中の弾を全て調べる

	for (i = 0; i < MAX_BULLET; i++)
	{
		if (!bullet[i].isExist)
			continue;

		x = bullet[i].x;
		y = bullet[i].y;

		angle = bullet[i].angle;
		angle += (TargetAnglePlayer(x, y) - bullet[i].angle) / fabs(TargetAnglePlayer(x, y) - bullet[i].angle) / (2 * M_PI) * 360;
		bullet[i].angle = angle;

		if (bullet[i].time >= 0)
		{
			bullet[i].accelerationX = 2 * (player.x - x - bullet[i].velocityX * bullet[i].time) / (bullet[i].time * bullet[i].time);
			bullet[i].accelerationY = 2 * (player.y - y - bullet[i].velocityY * bullet[i].time) / (bullet[i].time * bullet[i].time);

			//if (bullet[i].accelerationX > 40) bullet[i].accelerationX = 40;
			//if (bullet[i].accelerationY > 40) bullet[i].accelerationY = 40;
		}
		bullet[i].velocityX += bullet[i].accelerationX * dt;
		bullet[i].velocityY += bullet[i].accelerationY * dt;

		bullet[i].time -= dt;

		//角度angle方向にspeed分進める
		x += bullet[i].speed * cos(angle);
		y += bullet[i].speed * sin(angle);

		//弾が画面外に出た場合、弾を消す
		if (x < MIN_X || x > MAX_X || y < MIN_Y || y > MAX_Y)
		{
			bullet[i].isExist = false;
			bullet[i].accelerationX = 0;
			bullet[i].accelerationY = 0;
			bullet[i].velocityX = 0;
			bullet[i].velocityY = 0;
			bullet[i].time = 2;
		}
		
		
		bullet[i].x += bullet[i].velocityX * dt;
		bullet[i].y += bullet[i].velocityY * dt;

		bullet[i].x += bullet[i].time/2 * bullet[i].speed * cos(angle);
		bullet[i].y += bullet[i].time/2 * bullet[i].speed * sin(angle);


		
	}

}


//敵の行動
void ActionEnemy()
{
	const int fire = 20;
	const double speed = 2.0;
	const double wide_angle = OMEGA(360);
	const int way = 5;
	//const double angle = TargetAnglePlayer(enemy.x, enemy.y);
	const double angle = OMEGA(0);
	const double range = 4;

	//敵の動き(今は固定)
	enemy.x = CENTER_X + 50 * cos(OMEGA(t));
	enemy.y = CENTER_Y/2 + 50 * sin(OMEGA(t));
 
	//fire回のループごとに弾を発射
	if( t % fire == 0 )
		MakeWayBullet(enemy.x, enemy.y ,speed, way, wide_angle, angle, range);
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
	DrawRotaGraph( enemy.x, enemy.y, 1.0, 0.0, enemy.img, TRUE ) ;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow )
{
	// タイトルを 変更
	SetMainWindowText( "方向弾" ) ;
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
		
		//自機の移動
		MovePlayer();

		//敵の行動
		ActionEnemy();

		//弾の移動
		MoveBullet2();

		//敵の表示
		DrawEnemy();

		//自機の表示
		DrawPlayer();

		//弾の表示
		DrawBullet();

		t++;//時間を進める

		// 裏画面の内容を表画面に反映
		ScreenFlip();
	}
	return 0;
}
