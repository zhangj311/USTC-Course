#include<stdio.h>
#include<string.h>
#include<math.h>
#include<algorithm>
#define MAXLENGTH 9
#define MAXCANNON 20
#define debug true
#define MAX 6
using namespace std;
bool mp[MAXLENGTH][MAXLENGTH];
int n, m, time, leftTime, hormoneMap[MAXLENGTH][MAXLENGTH];
int dx[4] = {0, 1, 0, -1}, dy[4] = {1, 0, -1, 0};
class ant
{
    friend class cannon;
    int hp, curHp, x, y, num, dir, act ;
public:
    static int curN, getCake, total;
    ant();
    void born(int i);
    void move();
    void info();
    void die();
    void hormone();
    bool isDead();
    bool isHome();
    void grow();
    bool operator<(const ant &a)const
    {
        return a.act > act;
    };
    bool isIntersect(int, int, int, int);
} ants[MAX];
int ant::curN = 0 ;
int ant::total = 0;
int ant::getCake = -1;

void ant::grow()
{
    ++act;
}
void ant::die()
{
    --curN;
    mp[x][y] = false;
}
bool ant::isHome()
{
    return x == 0 && y == 0;
}
bool ant::isDead()
{
    return curHp < 0;
}
ant::ant()
{
    x = y = 0;
    dir = -1;
    act = 1;
}
void ant::born(int j)
{
    if (curN == 6 || mp[0][0]) return ;

    ++ curN;
    mp[0][0] = true;
    num = total ++;
    curHp = hp = (int)(4 * pow(1.1, num / 6 + 1));
}
void ant::info()
{
    printf("%d %d %d %d %d\n", act - 1, num / 6 + 1, curHp, x, y);
}
void ant::hormone()
{
    if (getCake == num)
        hormoneMap[x][y] += 5;
    else
        hormoneMap[x][y] += 2;
}
void ant::move()
{
    bool canMove[4] = {true, true, true, true};

    if (dir != -1) {
        canMove[(dir + 2) % 4] = false;
    }

    int hormone = -1;
    bool stay = true;

    for (int i = 0; i < 4; ++i) {
        if (canMove[i]) {
            int tx = x + dx[i], ty = y + dy[i];  //wrongly type dx  ,and waste a lot of time

            if (tx < 0 || tx > n || ty < 0 || ty > m || mp[tx][ty]) {
                canMove[i]  = false;
                continue;
            }

            if (hormoneMap[tx][ty] > hormone) {
                hormone = hormoneMap[tx][ty];
                stay = false;
                dir = i;
            }
        }
    }

    if (stay) {
        dir = -1;
        return;
    }

    if (act % 5 == 0) {
        dir = (dir + 3) % 4;

        while (!canMove[dir])
            dir = (dir + 3) % 4;
    }

    mp[x][y] = false;
    x += dx[dir], y += dy[dir];
    mp[x][y] = true;

    if (getCake == -1 && x == n && y == m) {
        getCake = num;
        curHp += hp / 2;

        if (curHp > hp)curHp = hp;
    }
}
void show()
{
    sort(ants, ants + MAX);
    printf("%d\n", ant::curN);

    for (int i = 0; i < MAX; ++i)
        if (!ants[i].isDead())
            ants[i].info();
}
bool ant::isIntersect(int r1, int c1, int r2, int c2)
{
    if ((x - r1) * (x - r2) > 0 || (y - c1) * (y - c2) > 0)
        return false;

    float slope = (r1 - r2) / (float)(c1 - c2);
    float sum = slope * (y - c1) + r1 - x;
    sum = sum > 0 ? sum : -sum;

    if (sum / sqrt(1 + pow(slope, 2)) > 0.5) return false;

    return true;
}

class cannon
{
    int x, y;
public:
    static int num, power, span;
    void attack(int *);
    void focus(int *);
    void pos();
    float dis(int, int);
} cannons[MAXCANNON];

int cannon::num = 0;
int cannon::power = 0;
int cannon::span = 0;
void cannon::pos()
{
    scanf("%d%d", &x, &y);
    mp[x][y] = true;
}
float cannon::dis(int r, int c)
{
    return sqrt(pow(r - x, 2) + pow(c - y, 2));
}
void cannon::attack(int *p)
{
    int target = -1;
    float minD = MAXLENGTH * 2;

    for (int i = 0; i < ant::curN; ++i) {
        //δpΪ! ants[i].isDead(), wrong, waste lots of time
        float d = dis(ants[p[i]].x, ants[p[i]].y);

        if (d <= span && d < minD) {
            minD = d;
            target = p[i];
        }
    }

    if (target != -1)ants[target].curHp -= power;
}
void cannon::focus(int *alive)
{
    int tx = ants[ant::getCake].x, ty = ants[ant::getCake].y;
    float d = dis(tx, ty);

    if (d > span) this->attack(alive);
    else {
        for (int i = 0; i < ant::curN; ++i) {
            if (ants[alive[i]].isIntersect(x, y, tx, ty))
                ants[alive[i]].curHp -= power;
        }
    }
}
int init()
{
    scanf("%d%d", &n, &m);
    scanf("%d%d%d", &cannon::num, &cannon::power, &cannon::span);
    memset(mp, 0, sizeof(mp));
    memset(hormoneMap, 0, sizeof(hormoneMap));

    for (int i = 0; i < cannon::num; ++i) {
        cannons[i].pos();
    }

    scanf("%d", &time);
    leftTime = time;
}
int _1s()
{
    if (debug)show();

    for (int i = 0; i < MAX; ++i)
        if (ants[i].isDead())
            ants[i].born(i);

    sort(ants, ants + MAX);
    int alive[MAX], count = 0;

    for (int i = 0; i < MAX; ++i) {
        if (!ants[i].isDead()) {
            alive[count++] = i;
            ants[i].hormone();
        }
    }

    for (int i = 0; i < count; ++i) {
        ants[alive[i]].move();
    }

    for (int i = 0; i <= n; ++i)
        for (int j = 0; j <= m; ++j)
            if (hormoneMap[i][j] > 0) --hormoneMap[i][j];

    for (int i = 0; i < cannon::num; ++i) {
        if (ant::getCake != -1)cannons[i].focus(alive);
        else cannons[i].attack(alive);
    }

    for (int i = 0; i < count; ++i) {
        if (ants[alive[i]].isDead())
            ants[alive[i]].die();
    }

    if (ant::getCake != -1) {
        if (ants[ant::getCake].isHome())
            return 1;
        else if (ants[ant::getCake].isDead())
            ant::getCake = -1;
    }

    for (int i = 0; i < count; ++i)ants[alive[i]].grow();

    return 0;
}
int main(void)
{
    init();

    while (--leftTime >= 0) {
        if (debug)printf("%dth s\n", time - leftTime);

        if (_1s() == 1) {
            printf("Game over after %d seconds\n", time - leftTime);
            show();
            return 0;
        }
    }

    printf("The game is going on\n");
    show();
    return 0;
}
