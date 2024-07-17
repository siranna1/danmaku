/*
J4�Q�[���v���O���~���O
�V���[�e�B���O�Q�[���T���v��

������@
�ړ��F�����L�[
���@�V���b�g�FZ�L�[

�摜�f��
HamCorossam
http://homepage2.nifty.com/hamcorossam/

���ʉ��f��
�U�E�}�b�`���C�J�@�Y
http://osabisi.sakura.ne.jp/m2/


�h�b�g�G�c�[��
EDGE
http://takabosoft.com/edge

j2130�@�����Y��

�v���C���[�ƓG�Ƃ��ꂼ��̒e�ɐF�̗v�f������āA�قȂ�F���m����������悤�ɂ��܂����B
�㔼�ɏo�Ă���G�̐��𑝂₵�܂���

*/

#include "DxLib.h"
#define _USE_MATH_DEFINES
#include <math.h>

//�E�B���h�E�T�C�Y
#define MIN_X 32
#define MIN_Y 16
#define MAX_X 416
#define MAX_Y 464

//��ʂ̒��S���W
#define CENTER_X ((MIN_X + MAX_X)/2)
#define CENTER_Y ((MIN_Y + MAX_Y)/2)

//�p�x�̌v�Z(����rad)
#define OMEGA( t ) (t * M_PI / 180)

//�G�̍ő吔
#define MAX_ENEMY 50

//��ʓ��ɍő�1000���̒e
#define MAX_BULLET 2000

//���@�V���b�g�̍ő吔
#define MAX_PLAYER_SHOT 100

//�e�̎��
#define NORMAL 1//�~
#define LASER 2//���[�U�[

//�G�̍s���p�^�[��
#define STOP 0//��~
#define STRAIGHT 1//���i
#define CIRCLE 2//�~�^��

//�ő�G�t�F�N�g��
#define MAX_EFFECT MAX_BULLET

//�F
#define WHITE GetColor(255,255,255)//��

#define LEVEL_UP_SCORE 300 //�G1�@��100�_�A1000�Ȃ�10�@�Ń��x���A�b�v

#define BORN1 20 // �G�o���p�x
#define BORN2 1000 // �{�X�o���p�x
const int LEVEL_MAX_SCORE = 100000;

int t;//����

int enemy_img;//�G�̉摜
int enemyred_img;
int boss_img;
int bossblue_img;
int bullet_img1,bullet_img2;//�e�̉摜
int bulletblue_img1, bulletblue_img2;
int shot_img;//���@�V���b�g�̉�
int shotred_img;
int shotblue_img;
int playerred_img;
int playerblue_img;

int board_img;//�g�̉摜
int back_img;//�w�i�̉摜
int effect_img[17];//�G�t�F�N�g�̉摜
int score;//�X�R�A

int shot_snd;//���@�V���b�g���ˉ�
int bullet_snd;//�G�e���ˉ�
int bom_snd1;//������1
int bom_snd2;//������2
int up_snd;//�p���[�A�b�v��

enum color
{
	red = 0,
	blue = 1
};

color GetRandomColor()
{
	switch (GetRand(1))
	{
	case 0:
		return red;
		break;

	case 1:
		return blue;
		break;
	}
}

//���@
struct Player
{
	int x;//���W           
	int y;
	int img;//�摜
	int hp;//�c�@
	double range;//�����蔻��
	bool isDamage;//��e���Ȃ�true�A��e���łȂ��Ȃ�false
	int maxdistance;
	color col;
};

struct Player player;

//���@�V���b�g
struct PlayerShot
{
	double x;//���W
	double y;
	double angle;//�p�x
	double speed;//���x
	double range;//�����蔻��
	int img;//�摜
	int power;//�V���b�g�̈З�
	int distance;
	bool isExist;//���݂�����true�A���Ȃ�������false
	color col;
};

struct PlayerShot shot[MAX_PLAYER_SHOT];

//�G
struct Enemy
{
	double x;//���W
	double y;
	double speed;//���x
	double angle;//�p�x
	double range;//�����蔻��
	int img;//�摜
	int hp;//�G�̗̑�
	int action;//�G�̍s��
	bool isExist;//���݂�����true�A���Ȃ�������false
	bool isBoss;//�{�X�Ȃ�true
	color col;
};

struct Enemy enemy[MAX_ENEMY];
bool isBossExist = false;


//�X�̒e
struct Bullet
{

	double x;//���W
	double y;
	double speed;//���x
	double angle;//�p�x
	double range ;//�����蔻��
	int img;//�摜
	bool isExist;//���݂�����true�A���Ȃ�������false
	color col;

};

struct Bullet bullet[MAX_BULLET];//MAX_BULLET��Bullet

//�G�t�F�N�g
struct Effect
{
	int x;//���W
	int y;
	int img[20];//�摜
	int max_img;//�摜�̍ő吔
	int t;//�o�ߎ���
	bool isExist;//���݂�����true�A���Ȃ�������false
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
	enemy[i].col = GetRandomColor();
}
//������
void Init()
{
	int i;

	t=0;//���ԏ�����

	score=0;//�X�R�A������

	//���@��K���ȍ��W��
	player.x = CENTER_X;
	player.y = CENTER_Y * 1.5 ;

	player.hp = 5;//�c�@5����X�^�[�g

	player.range = 3;//�����蔻��
	player.maxdistance = 200;

	player.isDamage = false;

	//�G�̏�����
	for(i = 0; i < MAX_ENEMY; i++)
	{
		initEnemy(i);
	}

	//�e��S�ď�����
	//�G�t�F�N�g������
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

//�摜�E���t�@�C����ǂݍ��ފ֐�
void LoadData()
{
	//�摜�ǂݍ���
	player.img = LoadGraph("player.png");
	playerred_img = LoadGraph("playerred.png");
	playerblue_img = LoadGraph("playerblue.png");
	enemy_img = LoadGraph("smallenemy.png");
	enemyred_img = LoadGraph("smallenemyred.png");
	boss_img = LoadGraph("boss.png");
	bossblue_img = LoadGraph("bossblue.png");
	bullet_img1 = LoadGraph("bullet1.png");
	bulletblue_img1 = LoadGraph("bullet1blue.png");
	bullet_img2 = LoadGraph("bullet2.png");
	bulletblue_img2 = LoadGraph("bullet2blue.png");
	shot_img = LoadGraph("shot.png");
	shotblue_img = LoadGraph("shotblue.png");
	shotred_img = LoadGraph("shotred.png");
	board_img = LoadGraph("board.png");
	back_img = LoadGraph("back.png");
	LoadDivGraph("effect.png",17,8,3,64,64,effect_img);

	//���ǂݍ���
	shot_snd = LoadSoundMem("push07.wav");
	bullet_snd = LoadSoundMem("close09.wav");
	bom_snd1 = LoadSoundMem("bom01.wav");
	bom_snd2 = LoadSoundMem("bom10.wav");
	up_snd = LoadSoundMem("power00.wav");

	// ���ʂ̐ݒ�
	ChangeVolumeSoundMem( 255*0.1, shot_snd ) ;
	ChangeVolumeSoundMem( 255*0.5, bullet_snd ) ;

}

//�G�t�F�N�g�����֐�
void MakeEffect(int x, int y, int max)
{
	int i,j; 

	//�g�p���̃G�t�F�N�g�𒲂ׂ�
	for( i = 0; i < MAX_EFFECT; i++)
	{
		if( !effect[i].isExist )
			break;
	}

	if ( i == MAX_EFFECT )//����󂢂ĂȂ�
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

//���@�V���b�g�̐���
void MakeShot(double speed, double angle, int power, double range, color col)
{
	int i;

	//���˒��̃V���b�g�𒲂ׂ�
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
	shot[i].col = col;
	shot[i].distance = 0;

	shot[i].img = shot_img;
	switch (col)
	{
	case red:
		shot[i].img = shotred_img;
		break;
	case blue:
		shot[i].img = shotblue_img;
		break;
	}

	PlaySoundMem( shot_snd , DX_PLAYTYPE_BACK ) ;//���ˉ�

}

//�������Ɏ��@�V���b�g�𔭎�
void MakeWayShot(double speed, int power, double range, int way, double wide_angle, double main_angle, color col)
{
	int i; 

	double w_angle;
	
	if (way == 1)
	{
		MakeShot(speed, main_angle, power, range, col);
		return;
	}
	
	for( i = 0; i < way; i++)
	{
		if( wide_angle == OMEGA(360))
			w_angle = main_angle + i * wide_angle / way;//���ˊp�x
		else
			w_angle = main_angle + i * wide_angle / ( way - 1 ) - wide_angle / 2;//���ˊp�x

		MakeShot(speed,w_angle,power,range, col);
	}
}

bool ispushspace = false;

//���@�̈ړ�
void ActionPlayer()
{
	const int fire = 4;
	const int speed = 4;

	double s_speed = 8;
	double s_angle = OMEGA( -90 );
	double range = 16;

	int power = 1;

	int way = 5;


	if( player.isDamage )//��e���͓������Ȃ�
		return;
	//�����L�[�ňړ�
	if( CheckHitKey(KEY_INPUT_LEFT) )
		player.x -= speed;
	if( CheckHitKey(KEY_INPUT_RIGHT) )
		player.x += speed;
	if( CheckHitKey(KEY_INPUT_UP) )
		player.y -= speed;
	if( CheckHitKey(KEY_INPUT_DOWN) )
		player.y += speed;

	int changecolor = 10;
	if (CheckHitKey(KEY_INPUT_SPACE) && !ispushspace)
	{
		ispushspace = true;
		switch ((player.col + 1) % 2)
		{
		case 0:
			player.col = red;
			break;
		case 1:
			player.col = blue;
			break;
		}
	}
	if (!CheckHitKey(KEY_INPUT_SPACE))
	{
		ispushspace = false;
	}

	//�V���b�g������
	if( CheckHitKey(KEY_INPUT_Z)  && t % fire == 0 )
	{
		//�G��10�|�����Ƃ�+1����(�ő�6����)
		way = score > LEVEL_UP_SCORE * 5 ? way : score / LEVEL_UP_SCORE + 1;
		MakeWayShot(s_speed,power,range,way,OMEGA( (way - 1) * 20 ),s_angle, player.col);
	}

	//�ړ��𐧌�
	if( player.x < MIN_X )
		player.x = MIN_X;
	if( player.x > MAX_X )
		player.x = MAX_X;
	if( player.y < MIN_Y )
		player.y = MIN_Y;
	if( player.y > MAX_Y)
		player.y = MAX_Y;
}

//���@�V���b�g�̈ړ�
void MoveShot()
{
	int i;

	double x,y,angle,speed;

	//���˒��̃V���b�g�𒲂ׂ�
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

		shot[i].distance += sqrt(speed * cos(angle) * speed * cos(angle) + speed * sin(angle) * speed * sin(angle));
		if (shot[i].distance > player.maxdistance)
		{
			shot[i].isExist = false;
			
		}

		//�e����ʊO�ɏo���ꍇ�A�e������
		if( x < MIN_X || x > MAX_X || y < MIN_Y || y > MAX_Y)
			shot[i].isExist = false;

		shot[i].x = x;
		shot[i].y = y;

	}
}

//���@�V���b�g�̓����蔻�菈��
void JudgeShot()
{
	int i,j;
	double x,y;


	//���݂���G�𒲂ׂ�
	for(i = 0; i < MAX_ENEMY; i++)
	{

		if( !enemy[i].isExist )
			continue;

		//���˒��̃V���b�g��S�Ē��ׂ�
		for(j = 0; j < MAX_PLAYER_SHOT; j++)
		{
			if( !shot[j].isExist )
				continue;
			if (shot[j].col == enemy[i].col)
				continue;

			x = shot[j].x - enemy[i].x;
			y = shot[j].y - enemy[i].y;

			//�G�Ɏ��@�V���b�g���������
			if( hypot (x,y) < enemy[i].range + shot[j].range )
			{
				shot[j].isExist = false;

				enemy[i].hp -= shot[j].power;//�G�̗̑͂����炷

				//�G�̗̑͂������Ȃ�����G������
				if(enemy[i].hp < 0)
				{
					MakeEffect( enemy[i].x, enemy[i].y, 17);//�����G�t�F�N�g

					PlaySoundMem( bom_snd1 , DX_PLAYTYPE_BACK ) ;//������
					if (enemy[i].isBoss)
					{
						score += 2000;//�X�R�A���Z
						isBossExist = false;
					}
					else
						score += 100;//�X�R�A���Z
					initEnemy(i);
					if( score % LEVEL_UP_SCORE == 0 && score > 0 && score < LEVEL_UP_SCORE * 6)
						PlaySoundMem( up_snd , DX_PLAYTYPE_BACK ) ;//�p���[�A�b�v��
				}
			}

		}
	}
}

//���@�Ɋւ���\��
void DrawPlayer()
{
	int i;

	switch (player.col)
	{
	case red:
		player.img = playerred_img;
		break;
	case blue:
		player.img = playerblue_img;
		break;

	}

	//���@�̕\��
	DrawRotaGraphF( (float)player.x, (float)player.y, 1.0, 0, player.img, TRUE ) ;

	//���@�V���b�g�̕\��
	for(i = 0; i < MAX_PLAYER_SHOT; i++)
	{
		if( shot[i].isExist )
			DrawRotaGraphF( (float)shot[i].x, (float)shot[i].y, 1.0, shot[i].angle, shot[i].img, TRUE ) ;
	}
}

//���@�_���̊p�x
double TargetAnglePlayer(double x, double y)
{	
	return atan2( player.y - y, player.x - x);	
}

//�e���̐���

//�����e(x,y:���˒n�_, speed:���x, angle:�p�x)
void MakeBullet(double x, double y, double speed, double angle, double range, int img, color col)
{
	int i; 

	for( i = 0; i < MAX_BULLET; i++)
	{
		//bullet[i]���g�p����Ă��Ȃ���΃p�����[�^�ݒ��
		if( !bullet[i].isExist )
			break;
	}
	if ( i == MAX_BULLET )//����󂢂ĂȂ�
		return;

	bullet[i].isExist = true;

	//���˒n�_�̍��W
	bullet[i].x = x;
	bullet[i].y = y;

	bullet[i].angle = angle;//���ˊp�x
	bullet[i].speed = speed;//���x

	bullet[i].range = range;

	bullet[i].img = img;//�摜�̑��

	bullet[i].col = col;

	PlaySoundMem( bullet_snd , DX_PLAYTYPE_BACK ) ;//���ˉ�

}

//way�e(way:�������ɑł�, angle:��`�̊p�x, main_angle:��`���ǂ̕�����������)
void MakeWayBullet(double x, double y, double speed, int way, double wide_angle ,double main_angle, double range,int img, color col)
{
	int i; 

	double w_angle;

	for( i = 0; i < way; i++)
	{
		if( wide_angle == OMEGA(360))
			w_angle = main_angle + i * wide_angle / way;//���ˊp�x
		else
			w_angle = main_angle + i * wide_angle / ( way - 1 ) - wide_angle / 2;//���ˊp�x

		MakeBullet(x,y,speed,w_angle,range,img, col);
	}

}

//�e�̈ړ�
void MoveBullet()
{
	double x, y;
	double angle;
	int i;

	//���˒��̒e��S�Ē��ׂ�
	for(i = 0; i < MAX_BULLET; i++)
	{
		if( !bullet[i].isExist )
			continue;

		x = bullet[i].x;
		y = bullet[i].y;

		angle = bullet[i].angle;

		//�p�xangle������speed���i�߂�
		x += bullet[i].speed * cos( angle );
		y += bullet[i].speed * sin( angle );

		//�e����ʊO�ɏo���ꍇ�A�e������
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

	//���˒��̒e��S�Ē��ׂ�
	for(i = 0; i < MAX_BULLET; i++)
	{
		if( !bullet[i].isExist )
			continue;
		if (bullet[i].col == player.col)
			continue;

		x = bullet[i].x - player.x;
		y = bullet[i].y - player.y;

		//���@����e���łȂ��A�����蔻�肪�ڐG������
		if( hypot (x,y) < player.range + bullet[i].range && !player.isDamage)
		{
			MakeEffect(player.x, player.y, 17);
			bullet[i].isExist = false;
			player.isDamage = true;

			player.hp--;

			PlaySoundMem( bom_snd2, DX_PLAYTYPE_BACK ) ;//������
		}
	}
}

//�G�e��S�ď���
void EraseBullet()
{
	int i;

	for(i = 0; i < MAX_BULLET; i++)
		bullet[i].isExist = false;
}

//�G�̏o��
void MakeEnemy()
{
	int i;
	int enemyborn = (LEVEL_MAX_SCORE - score) > 0 ? (LEVEL_MAX_SCORE - score) * 40 / LEVEL_MAX_SCORE + BORN1 - 10 : BORN1 - 10;
	enemyborn = BORN1 - score / 1000;
	if (enemyborn < 5)
		enemyborn = 5;
	//�U�R�G ���������_�������ɒ��i
	if( t % enemyborn == 0 && !isBossExist )
	{
		for(i = 0;i < MAX_ENEMY; i++)
		{
			if( !enemy[i].isExist )
				break;
		}

		if( i == MAX_ENEMY )
			return ;

		enemy[i].isExist = true;

		//��ԏ�̃����_���Ȉʒu�ɏo��
		enemy[i].x = GetRand(MAX_X - MIN_X) + MIN_X;
		enemy[i].y = MIN_Y;

		enemy[i].hp = 1;

		enemy[i].angle = OMEGA( GetRand(180) );//�p�x��0���`180��
		enemy[i].speed = GetRand(6) + 2;//���x��2�`8

		enemy[i].range = 15;//�����蔻��̑傫��

		enemy[i].action = STRAIGHT;//���܂����p�x�ɒ��i
		switch (GetRand(1))
		{
		case 0:
			enemy[i].action = STRAIGHT;
			break;
		case 1:
			enemy[i].action = CIRCLE;
			break;
		}
		
		enemy[i].img = enemy_img;

		switch (enemy[i].col)
		{
		case red:
			enemy[i].img = enemyred_img;
			break;
		case blue:
			enemy[i].img = enemy_img;
			break;
		}
	}

	//��^�G ���W�Œ�
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

		enemy[i].angle = OMEGA( 90 );//������
		enemy[i].speed = 0;

		enemy[i].range = 130;

		enemy[i].action = STOP;//���W�Œ�

		enemy[i].img = boss_img;
		switch (enemy[i].col)
		{
		case red:
			enemy[i].img = boss_img;
			break;
		case blue:
			enemy[i].img = bossblue_img;
			break;
		}
	}
}


//�G�̍s��
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

		if( !enemy[i].isExist )//�G������Ύ���
			continue;

		switch (enemy[i].action)
		{
		case STOP://�������Ȃ�
			enemy[i].x += sin(OMEGA(t));
			break;

		case STRAIGHT://���܂��������ɒ��i

			enemy[i].x += enemy[i].speed * cos(enemy[i].angle);
			enemy[i].y += enemy[i].speed * sin(enemy[i].angle);

			break;
		case CIRCLE:
			enemy[i].angle += OMEGA(1);

			enemy[i].x += enemy[i].speed * cos(enemy[i].angle);
			enemy[i].y += enemy[i].speed * sin(enemy[i].angle);
			break;
		}
		

		x = enemy[i].x;
		y = enemy[i].y;

		//�e����ʊO�ɏo���ꍇ�A�e������
		if( x < MIN_X || x > MAX_X || y < MIN_Y || y > MAX_Y)
			enemy[i].isExist = false;

		int img1, img2;
		switch (enemy[i].col)
		{
		case red:
			img1 = bullet_img1;
			img2 = bullet_img2;
			break;
		case blue:
			img1 = bulletblue_img1;
			img2 = bulletblue_img2;
		}
		switch(enemy[i].action)
		{
		case STOP:
			if (score > LEVEL_MAX_SCORE)
				fire = 5;
			else
				fire = (LEVEL_MAX_SCORE - score) * 40 / LEVEL_MAX_SCORE + 5;
			x = enemy[i].x;
			y = enemy[i].y;
			speed = 2.0;
			way = 5;
			range = 8;
			//fire��̃��[�v���Ƃɒe�𔭎�
			if( t % fire == 0 )//3��ނ̒e��
			{
				
				MakeWayBullet(x+140,y,1.5,3,OMEGA( 60 ),OMEGA(90)+OMEGA(30)*sin(OMEGA(t)),3,img2, enemy[i].col);
				MakeWayBullet(x-140,y,1.5,3,OMEGA( 60 ),OMEGA(90)+OMEGA(30)*sin(OMEGA(t)),3,img2, enemy[i].col);
				MakeWayBullet(x,y,speed,way,OMEGA(5),TargetAnglePlayer(x,y),range,img1, enemy[i].col);
			}
			break;

		case STRAIGHT:
			
			if( score > LEVEL_MAX_SCORE )
				fire = 20;
			else
				fire = (LEVEL_MAX_SCORE - score) * 60 / LEVEL_MAX_SCORE + 20;
			x = enemy[i].x;
			y = enemy[i].y;
			speed = 2.0;
			way = 3;
			range = 8;
			//fire��̃��[�v���Ƃɒe�𔭎�
			if( t % fire == 0 )
			{
				switch(GetRand(1))//�ǂ��炩�̒e���𔭎�
				{
				case 0:
					MakeWayBullet(x,y,speed,way,OMEGA(30),TargetAnglePlayer(x,y),range,img1, enemy[i].col);
					break;

				case 1:
					MakeWayBullet(x,y,speed,2*way,OMEGA(360),OMEGA( GetRand(360) ),range,img1, enemy[i].col);
					break;
				}
			}
			break;

		}
	}
}

//�e�̕\��
void DrawBullet()
{
	double x,y,angle;
	int img;

	int i;

	//���˒��̒e��S�Ē��ׂ�
	for(i = 0; i < MAX_BULLET; i++)
	{
		if( !bullet[i].isExist )
			continue;

		x = bullet[i].x ;
		y = bullet[i].y ;

		angle = bullet[i].angle ;

		img = bullet[i].img ;

		//�e�̕\��
		DrawRotaGraphF( (float)x, (float)y, 1.0, angle, img, TRUE ) ;

	}
}

//�G�̕\��
void DrawEnemy()
{
	int i;

	for( i = 0; i < MAX_ENEMY; i++ )
	{
		if( !enemy[i].isExist )//�G������Ύ���
			continue;

		DrawRotaGraph( enemy[i].x, enemy[i].y, 1.0, enemy[i].angle, enemy[i].img, TRUE ) ;

	}

}

//�G�t�F�N�g�̕\��
void DrawEffect()
{
	int i;
	int x,y;

	int ft;

	//���˒��̃G�t�F�N�g��S�Ē��ׂ�
	for(i = 0; i < MAX_EFFECT; i++)
	{
		if( !effect[i].isExist )
			continue;

		x = effect[i].x ;
		y = effect[i].y ;
		ft = effect[i].t ;

		DrawRotaGraphF( (float)x, (float)y, 1.0, 0, effect[i].img[ft], TRUE ) ;

		effect[i].t++ ;//�G�t�F�N�g�̎��Ԃ�i�߂�

		//�S���\�����I����������
		if(effect[i].t >= effect[i].max_img)
		{
			effect[i].isExist = false;

			if(effect[i].x == player.x && effect[i].y == player.y)//���@��e�̏ꍇ
			{
				EraseBullet();//�G�e�����ׂď���
				player.isDamage = false;//��e���ł͂Ȃ��Ȃ�
			}
		}
	}
}

//�w�i�̕\��

void DrawBack()
{
	int time;

	time = 8 * ( t % ( ( MAX_Y - MIN_Y ) / 8 ) );

	//�w�i�X�N���[���\��

	DrawGraph( MIN_X, MIN_Y + time, back_img, false );

	if( time > 0 )
		DrawGraph( MIN_X, 2 * MIN_Y + time - MAX_Y, back_img, false );
}

//�X�R�A���̕\��
void DrawSystem()
{
	DrawGraph( 0, 0, board_img,true);

	DrawFormatString(MAX_X+50,MIN_Y+30,WHITE,"PLAYER�F%d",player.hp);
	DrawFormatString(MAX_X+50,MIN_Y,WHITE,"SCORE�F%d",score);
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow )
{
	// �^�C�g���� �ύX
	SetMainWindowText( "�V���[�e�B���O�T���v��" ) ;
	// �E�C���h�E���[�h�ɕύX
	ChangeWindowMode( TRUE ) ;
	// �c�w���C�u��������������
	if( DxLib_Init() == -1 )		// �c�w���C�u��������������
		return -1 ;			// �G���[���N�����璼���ɏI��

	// �`����ʂ𗠂ɂ���
	SetDrawScreen(DX_SCREEN_BACK);

	while(1)
	{	
		LoadData();
		Init();

		DrawString(0,330,"Press Space Bar",WHITE);//�X�y�[�X�L�[�������ĊJ�n
		ScreenFlip();
		while(!CheckHitKey(KEY_INPUT_SPACE))
		{
			if ( ProcessMessage() == -1 )
			{
				DxLib_End();// �c�w���C�u�����g�p�̏I������
				return 0;
			}		//���C�����[�v Esc�L�[�ŏI��
		}
		while( !CheckHitKey(KEY_INPUT_ESCAPE) &&  player.hp > 0)
		{
			if ( ProcessMessage() == -1 )
			{
				DxLib_End();// �c�w���C�u�����g�p�̏I������
				return 0;
			}

			ClearDrawScreen();

			//���@�̈ړ�
			ActionPlayer();

			//���@�V���b�g�̈ړ�
			MoveShot();

			//�G�̔���
			MakeEnemy();

			//�G�̍s��
			ActionEnemy();

			//�e�̈ړ�
			MoveBullet();

			//���@�V���b�g�̓����蔻�菈��
			JudgeShot();

			//�G�e�̓����蔻�菈��
			JudgeBullet();

			//�w�i�̕\��
			DrawBack();

			//�G�̕\��
			DrawEnemy();

			//���@�̕\��
			DrawPlayer();

			//�G�t�F�N�g�\��
			DrawEffect();

			//�e�̕\��
			DrawBullet();

			DrawSystem();

			t++;//���Ԃ�i�߂�

			// ����ʂ̓��e��\��ʂɔ��f
			ScreenFlip();
		}

		DrawString(0,300,"GAME OVER",WHITE);

		ScreenFlip();
	}
	DxLib_End();				// �c�w���C�u�����g�p�̏I������
	return 0;
}
