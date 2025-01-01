#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <stack>

int problemnum;//总题数量
int teamnum;//队伍数量

struct frozen { //判断各支队伍各道题是否处于冻结状态的结构体

    bool existfreeze=false;//是否存在冻结状态下的题目

    bool isfreeze[26]={false};

    int submitadd[26] = {0};//用于判断在封榜后各队伍提交题目的次数

    int submitAC[26] = {0};//封榜后各支队伍经过多少次提交AC(前提是封榜前队伍并没有AC这道题

    int ACtime[26] = {0};
};

struct information {//关于一个队伍的全部信息
    std :: string name;
    int ACnum = 0;//AC数量 可以直接保存在这里而不用再经过detail结构体

    int finet = 0;//罚时总时长
    std :: vector<int> maxt;//最大通过时长 初始状态均设为0

    int submitt[26] = {0};//每一道题的当前提交次数


    int situation[26] = {0};//每一道题的当前提交状态，若AC则1 WA则2 RE则3 TLE则4 （必要性存疑，先留在这里（似乎必要
    int fake[26] = {0};// 每一题的伪提交状态 若AC则1 WA则2 RE则3 TLE则4 （反应的是每题实际的最后一次提交状态

    int AC[26] = {0};//每一道题达到AC状态时所经历的时间（只有AC时才需要对这个变量进行存储，其余情况无需考虑
    int WA[26] = {0};//每一道题最后一次达到WA状态时所经历的时间（只有WA时才需要对这个变量进行存储，其余情况无需考虑 不断更新
    int RE[26] = {0};//每一道题最后一次达到RE状态时所经历的时间（只有RE时才需要对这个变量进行存储，其余情况无需考虑 不断更新
    int TLE[26] = {0};//每一道题最后一次达到TLE状态时所经历的时间（只有TLE时才需要对这个变量进行存储，其余情况无需考虑 不断更新

    int lastt=0;//最后一次提交的时间
    int laststatus = 0;//最后一次提交状态 数组存储 可能存在同时提交多题
    char lastproblem = '0';//最后一次提交问题序号 数组存储 可能存在同时提交多题

    //保存最后一次AC,WA,RE,TLE的题目序号 可能存在同时提交多题
    char lastAC ='0';
    char lastWA ='0';
    char lastRE ='0';
    char lastTLE ='0';
};


//完全同步
//std :: unordered_map<int,std :: string> infork;//用于存放序号对应的队伍名称 序号到名称的映射 fork的逆映射

std :: unordered_map<std::string,int> fork;//用于存放队伍对应的序号 名称到序号的映射

std :: vector<information> message;//vector存放对于排行有用的信息

std :: vector<frozen> freezing;//判断各支队伍各道题是否处于冻结状态的vector

std :: vector<std :: string> infork; //用于存放序号对应的队伍名称 序号到名称的映射 fork的逆映射

struct compare { //set排序方式
    bool operator()(int a,int b) const {
        if(a==b) return false;
        if(message[a].ACnum != message[b].ACnum) return message[a].ACnum > message[b].ACnum;
        if(message[a].finet != message[b].finet) return message[a].finet < message[b].finet;
        //对maxt进行从大到小的排序
        //  todo todo todo todo
        //std :: stack<int> tmp1;
        //std :: stack<int> tmp2;
        //std :: sort(message[a].maxt.begin() ,message[a].maxt.end());
        //std :: sort(message[b].maxt.begin() ,message[b].maxt.end());
        for(int i = message[a].ACnum-1;i >= 0; i--) {
            if(message[a].maxt[i] != message[b].maxt[i]) {
                return message[a].maxt[i] < message[b].maxt[i];
            }
        }
        return message[a].name < message[b].name;
    }
};

//下面都是与排名相关的容器
std :: set< int , compare> currentranking;//用于对数组message进行排序 对于实时排行榜 只是一个普通的排序工具 中转站性质

std :: vector<int> studentranking;// 存储 studentranking[0]=8是指0号队当前的排名为8 人到排名 旧排名

std :: vector<int> rankingstudent;//  主要用于printlist的操作 rankingstudent[0]=8是指排名为0（1）的是8号队 排名到人 旧排名


std :: vector<int> maxtextra; // 用于解决滚榜时不满足保序性的问题 //todo remshed

int main() {
    //输入输出优化似乎会产生一些神奇的bug,姑且不使用
    std :: ios::sync_with_stdio(false);
    std :: cin.tie(nullptr);
    //freopen("C:\\in.txt","r",stdin); //输入重定向，输入数据将从D盘根目录下的in.txt文件中读取
    //freopen("C:\\out.txt","w",stdout); //输出重定向，输出数据将保存在D盘根目录下的out.txt文件中


    std :: string s ;
    bool startsign = false;//开始标志 初始状态下为false
    bool freezesign = false;//封榜标志 初始状态下为false

    while( true ) {
        std :: cin >> s ;
        if( s == "ADDTEAM" ) {
            if( !startsign ){
                std :: string name;
                std :: cin >> name;
                if(fork.find(name) == fork.end()) {  //二分查找时间复杂度logN
                    std :: cout << "[Info]Add successfully.\n";
                    fork.insert({name,fork.size()});//把队伍名存入fork map中
                    //这里直接固定fork这个映射，int全部按照顺序排列
                    infork.push_back(name);//可以看作是fork的逆映射

                }else std :: cout << "[Error]Add failed: duplicated team name.\n";
            }else std :: cout << "[Error]Add failed: competition has started.\n";

        }else if( s == "START") {
        //START DURATION [duration_time] PROBLEM [problem_count]

            //判断是否开始
            if(!startsign) {
                teamnum = fork.size();

                int durationtime;
                //输入部分
                std :: string none1,none2;
                std :: cin>>none1;
                std :: cin>>durationtime;
                std :: cin>>none2;
                std :: cin>>problemnum;
                startsign = true;
                std :: cout << "[Info]Competition starts.\n";

                //5个vector 初始化
                message.resize(teamnum);
                freezing.resize(teamnum);
                studentranking.resize(teamnum);
                rankingstudent.resize(teamnum);

                for(int i = 0 ;i < teamnum; i++ ) {
                    message[i].name = infork[i];
                }

                for(int i = 0;i < teamnum; i++ ) {
                    currentranking.insert(i);//在currentranking中存入0~i-1的数字（最后输出排名+1 1~based
                }

                //初始存入一些排名
                int i=0;
                for(auto it = currentranking.begin();i < teamnum; i++ , it++ ) {
                    rankingstudent[i] = *it;//得到0~n-1的序列分别对应的排名 初始排名全部依据队伍名称的字典序
                    studentranking[*it] = i;//得到0~n-1排名对应的队伍名
                }
            }else {
                std :: string none;
                std :: getline(std :: cin,none,'\n');

                std :: cout << "[Error]Start failed: competition has started.\n";
            }


        }else if(s == "SUBMIT") { //相当于update的操作 (注意要考虑冻结题目的情况

            //todo 对存在冻结状态的题目进行在修改
            //SUBMIT [problem_name] BY [team_name] WITH [submit_status] AT [time]

            //输入部分
            char letter;
            std :: string name;
            std ::string status;
            int times;
            std :: cin >> letter; //题号
            std :: string none1 ,none2 ,none3 ;
            std :: cin >> none1;//吞字
            std :: cin >> name;//队伍名
            std :: cin >> none2;//吞字
            std :: cin >> status;//提交状态
            std :: cin >> none3;//吞字
            std :: cin >> times;//提交时间
            int serial = letter-'A';//将所有字母题号转化为数字
            //先对set中队伍编号所在位置的元素进行删除
            currentranking.erase(currentranking.find(fork[name]));//删除原先的排名

            //最后一次提交的题目编号（用于query_submission 无关是否封榜
            message[fork[name]].lastproblem = letter;


            //query_submission(无关是否封榜
            if(status == "Accepted") {
                message[fork[name]].fake[serial] = 1;
                message[fork[name]].lastAC = letter;
                message[fork[name]].laststatus = 1;


            }else if(status == "Wrong_Answer") {
                message[fork[name]].fake[serial] = 2;
                message[fork[name]].lastWA = letter;
                message[fork[name]].laststatus = 2;

            }else if(status == "Runtime_Error") {
                message[fork[name]].fake[serial] = 3;
                message[fork[name]].lastRE = letter;
                message[fork[name]].laststatus = 3;


            }else if(status == "Time_Limit_Exceed") {
                message[fork[name]].fake[serial] = 4;
                message[fork[name]].lastTLE = letter;
                message[fork[name]].laststatus = 4;


            }

            message[fork[name]].lastt = times;//仅供查询submission(无论是否封榜与否

            //对于几种状态均需讨论冻结的状态
            if(status == "Accepted") {
                //for querysubmission
                message[fork[name]].AC[serial] = times;  //更新AC时间，无论什么情况下都必不可少

                if(freezesign) {
                    //todo todo todo todo
                    //对freezing数组相应位置进行一些改动
                    ++ freezing[fork[name]].submitadd[serial];//将封榜后的提交次数另外存起来
                    if(message[fork[name]].situation[serial] != 1) { //若本来就已经AC了此题，则直接忽视此次操作（只要在最后提交系列信息中记录相关内容
                        //若已经AC后再次AC则不记录为冻结状态
                        message[fork[name]].situation[serial] = 1; //状态计入，无论什么情况下都必不可少
                        freezing[fork[name]].existfreeze = true;//存在冻结状态下的题目
                        freezing[fork[name]].isfreeze[serial] = true;//将这道题设为冻结状态
                        freezing[fork[name]].ACtime[serial] = times;

                    }
                }else {
                    //几个与排名直接挂钩的变量只有在未解冻状态才可以被修改
                    //注意： 已经AC则不需要增加submit[serial]
                    if(message[fork[name]].situation[serial] != 1) { //若本来就已经AC了此题，则直接忽视此次操作

                        message[fork[name]].situation[serial] = 1; //状态计入，无论什么情况下都必不可少
                        ++ message[fork[name]].ACnum ;//若AC则AC总数加一 仅供排名
                        message[fork[name]].maxt.push_back(times);//仅供排名

                        message[fork[name]].finet += times + 20*message[fork[name]].submitt[serial]; //累加罚时 仅供排名（但滚榜时未通过仍需要计入

                    }
                    //得到的新的current Ranking为经过一次提交后比赛场上新的实时排名
                    //若处于封榜状态中，current Ranking也不进行更新 将更新的操作放在scroll中进行
                }
                //将结果重新插入current Ranking中
                currentranking.insert(fork[name]);//重新插入计算排名
            }else { // i.e 没有AC
                //更新每题的提交状态
                if(status == "Wrong_Answer") {
                    if(message[fork[name]].situation[serial] != 1) {
                        message[fork[name]].situation[serial] = 2;
                    }
                    message[fork[name]].WA[serial] = times;//更新WA时间
                }else if(status =="Runtime_Error") {
                    if(message[fork[name]].situation[serial] != 1) {
                        message[fork[name]].situation[serial] = 3;
                    }
                    message[fork[name]].RE[serial] = times;//更新RE时间

                }else if(status =="Time_Limit_Exceed") {
                    if(message[fork[name]].situation[serial] != 1) {
                        message[fork[name]].situation[serial] = 4;
                    }
                    message[fork[name]].TLE[serial] = times;//更新TLE时间

                }

                //接着要将一些封榜后提交的题目进行冻结操作
                //todo todo todo todo


                if(freezesign) { //如果现在处于封榜状态 冻结状态下提交的题目将不会显示在实时排名中 之后输出的排名都为封榜前最后一次刷新的排名
                    ++ freezing[fork[name]].submitadd[serial];//将封榜后的提交次数另外存起来
                    if(message[fork[name]].situation[serial] != 1) { //此题一直处于没有AC的状态（不包括一些重复提交的唐氏行为
                        freezing[fork[name]].existfreeze = true;//存在冻结状态下的题目
                        freezing[fork[name]].isfreeze[serial] = true;//将这道题设为冻结状态
                        ++ freezing[fork[name]].submitAC[serial];//AC前次数的累加

                    }
                }else {
                    if(message[fork[name]].situation[serial] != 1) {
                        ++ message[fork[name]].submitt[serial];
                    }//提交次数增加 只需要增加提交的次数以达到计算罚时的目的即可
                    //得到的新的current Ranking为经过一次提交后比赛场上新的实时排名

                }

                currentranking.insert(fork[name]);//重新插入计算排名 若为封榜状态下，这一步操作其实对结果并没有产生影响，因为影响排名的变量都被冻住了
            }
            // 更新排名

        }else if(s == "FLUSH") {//刷新榜单
            //此步操作为将current Ranking 赋给rankingstudent studentranking

            std ::cout<<"[Info]Flush scoreboard.\n";
            int i=0;
            for(auto it = currentranking.begin();i < teamnum; i++ , it++ ) {
                rankingstudent[i] = *it;//得到0~n-1的序列分别对应的排名 初始排名全部依据队伍名称的字典序
                studentranking[*it] = i;//得到0~n-1排名对应的队伍名
            }
            //排名更新

        }else if(s == "FREEZE") { //封榜操作

            if( !freezesign ) {//处于未封榜状态 滚帮后要解除这一状态
                freezesign = true;
                std :: cout << "[Info]Freeze scoreboard.\n";
                //从此刻开始查询排名的都将是rankinglist的结果

            }else std :: cout << "[Error]Freeze failed: scoreboard has been frozen.\n";//处于封榜状态（还未滚榜
            //应该没什么要加的了，待定

        }else if(s == "SCROLL") {
            if( !freezesign ) { //若不处于封榜状态 则无法进行滚榜操作

                std :: cout<<"[Error]Scroll failed: scoreboard has not been frozen.\n";

            }else {
                std :: cout<<"[Info]Scroll scoreboard.\n";//提示词
                //滚榜前先输出一次榜单 用rankingstudent即可 为封榜时的那一次刷新得到的排名
                //队名 排名 解题数 总罚时 A B C ...

                int k=0;
                for(auto it = currentranking.begin();k < teamnum; k++ , it++ ) {
                    rankingstudent[k] = *it;//得到0~n-1的序列分别对应的排名 初始排名全部依据队伍名称的字典序
                    studentranking[*it] = k;//得到0~n-1排名对应的队伍名
                }

                for(int i = 0;i < teamnum; i++ ) {
                    //          队名  infork映射得到string                                   AC数量                                  罚时
                    std :: cout<<infork[rankingstudent[i]]<<" "<<i+1<<" "<<message[rankingstudent[i]].ACnum<<" "<<message[rankingstudent[i]].finet<<" ";

                    for(int j = 0;j < problemnum; j++) {
                        // ABC...各道题的提交情况
                        if(message[rankingstudent[i]].situation[j] == 1 && !freezing[rankingstudent[i]].isfreeze[j]) {//若这一题AC且不处于冻结状态
                            if(message[rankingstudent[i]].submitt[j] == 0 ) {

                                std :: cout<<"+"<<" "; //若一遍AC，则输出+
                            }else {
                                std :: cout<<"+"<<message[rankingstudent[i]].submitt[j]<<" ";
                            }
                        }else if(message[rankingstudent[i]].situation[j] != 1&& !freezing[rankingstudent[i]].isfreeze[j]) { //若这题没有AC且不处于冻结状态

                            if(message[rankingstudent[i]].submitt[j] == 0) { //从来没有提交过
                                std :: cout<<"."<<" ";
                            }else {
                                std :: cout<<"-"<<message[rankingstudent[i]].submitt[j]<<" ";//输出提交次数
                            }

                        }else if(freezing[rankingstudent[i]].isfreeze[j]) { //这一题处于冻结状态
                            if(message[rankingstudent[i]].submitt[j] == 0) { //说明此题在封榜前从未提交过

                                std :: cout<<0<<"/"<<freezing[rankingstudent[i]].submitadd[j]<<" ";
                            }else { //在封榜前有提交记录但封榜前没有AC //todo若AC最后提交次数应该-1
                                std :: cout<<"-"<<message[rankingstudent[i]].submitt[j]<<"/"<<freezing[rankingstudent[i]].submitadd[j]<<" ";
                            }
                        }
                    }

                    std :: cout<<"\n";
                }

                //每一行输出滚榜时 导致排名变化 的解冻
                //todo 滚榜造成的位次变化
                int number;
                int number2 ;//先存最后一名和倒数名的队伍编号
                //auto it = currentranking.find(rankingstudent[teamnum-1]);
                while (!currentranking.empty()) {
                    auto it = --currentranking.end();
                    int number = *it; // 保存当前队伍编号
                    currentranking.erase(it); // 移除元素
                    if (!freezing[number].existfreeze) {
                        continue;
                    } else {
                        bool flag = false;
                        for (int j = 0; j < problemnum; j++) {
                            if (j == problemnum - 1) {
                                freezing[number].existfreeze = false;
                            }
                            if (freezing[number].isfreeze[j]) {
                                freezing[number].isfreeze[j] = false;

                                if (message[number].situation[j] != 1) {
                                    message[number].submitt[j] += freezing[number].submitadd[j];
                                    freezing[number].submitadd[j] = 0;
                                    freezing[number].submitAC[j] = 0;
                                } else {
                                    message[number].submitt[j] += freezing[number].submitAC[j];
                                    freezing[number].submitadd[j] = 0;
                                    freezing[number].submitAC[j] = 0;
                                    ++message[number].ACnum;
                                    int position = 0;
                                    for (int k = message[number].maxt.size() - 1; k >= 0; k--) {
                                        if (message[number].maxt[k] <= freezing[number].ACtime[j]) {
                                            position = k + 1;
                                            break;
                                        }
                                    }
                                    message[number].maxt.insert(message[number].maxt.begin() + position, freezing[number].ACtime[j]);
                                    message[number].finet += freezing[number].ACtime[j] + 20 * message[number].submitt[j];
                                    int nowfinet = message[number].finet;

                                    auto it4 = currentranking.upper_bound(number); //找下一位
                                    if (it4 != currentranking.end() && *it4 != number) {
                                        flag = true;
                                        int oldowner = *it4;
                                        std::cout << infork[number] << " " << infork[oldowner] << " " << message[number].ACnum << " " << message[number].finet << "\n";
                                        break;
                                    }
                                }
                            }
                        }
                        if (flag) {
                            currentranking.insert(number);
                        }
                    }
                }
                for(int i = 0;i < teamnum; i ++) {
                    currentranking.insert(i);
                }
                //输出滚榜操作结束后最后得到的排名  现在没有题目处于冻结状态
                int m = 0;
                for(auto it1 = currentranking.begin();m < teamnum; m++, it1++) {//时间复杂度N

                    rankingstudent[m] = *it1;//得到0~n-1的序列分别对应的排名
                    studentranking[*it1] = m;//得到0~n-1的排名对应的队伍

                }
                for(int i = 0;i < teamnum; i++ ) {
                    //队名 排名 解题数 总罚时 A B C ...

                    std :: cout<<infork[rankingstudent[i]]<<" "<<i+1<<" "<<message[rankingstudent[i]].ACnum<<" "<<message[rankingstudent[i]].finet<<" ";
                    for(int j = 0; j < problemnum; j++ ) {
                        // ABC...各道题的提交情况
                        if(message[rankingstudent[i]].situation[j] == 1) { //这一题AC

                            if(message[rankingstudent[i]].submitt[j] == 0) {
                                std :: cout<<"+"<<" ";//若一遍AC，则输出+
                            }else {
                                std :: cout<<"+"<<message[rankingstudent[i]].submitt[j]<<" ";
                            }

                        }else if(message[rankingstudent[i]].situation[j] != 1) { //若这题没有AC

                            if(message[rankingstudent[i]].submitt[j] == 0) {
                                std :: cout<<"."<<" ";
                            }else {
                                std :: cout<<"-"<<message[rankingstudent[i]].submitt[j]<<" ";
                            }

                        }
                    }
                    std :: cout<<"\n";
                }
                freezesign = false;//解封，可以继续封榜了
            }
        }else if(s == "QUERY_RANKING") {

            std :: string name;
            std :: cin>>name;

            if(fork.find(name) == fork.end()) { //o1复杂度
                std :: cout<<"[Error]Query ranking failed: cannot find the team.\n";
            }else {
                std :: cout<<"[Info]Complete query ranking.\n";
                if(freezesign) {
                    std :: cout<<"[Warning]Scoreboard is frozen. The ranking may be inaccurate until it were scrolled.\n";
                }
                std :: cout<<name<<" NOW AT RANKING "<<studentranking[fork[name]]+1<<"\n";//1-based
            }

        }else if(s == "QUERY_SUBMISSION") {
            //[team_name] [problem_name] [status] [time]
            //QUERY_SUBMISSION [team_name] WHERE PROBLEM=[problem_name] AND STATUS=[status]

            std :: string name;
            std :: cin >> name;
            std :: string none1,none2;
            std :: getline(std :: cin >> none1,none1,'=');
            std :: string problemname;
            std :: cin >> problemname;
            std :: getline(std :: cin >> none1,none2,'=');
            std :: string status;
            std :: cin >> status;

            if(fork.find(name) == fork.end()) { //logN复杂度
                std :: cout << "[Error]Query submission failed: cannot find the team.\n";
            }else {
                std :: cout << "[Info]Complete query submission.\n";

                if(problemname == "ALL") { //对所有题目进行查询
                    //TODO 需要改动 可能同时交了同一道题 TODO TODO TODO
                    if(status == "ALL") { //查询队伍的最后一次提交
                        if(message[fork[name]].lastt == 0) { //最后一次提交时间为0
                            std :: cout<<"Cannot find any submission.\n";//从来没有交过任何一道题
                        }else {
                            std :: string lastsituation; //将laststatus翻译成string
                            if(message[fork[name]].laststatus == 1) {
                                lastsituation = "Accepted";
                            }else if(message[fork[name]].laststatus == 2) {
                                lastsituation = "Wrong_Answer";
                            }else if(message[fork[name]].laststatus == 3) {
                                lastsituation = "Runtime_Error";
                            }else if(message[fork[name]].laststatus == 4) {
                                lastsituation = "Time_Limit_Exceed";
                            }

                            std :: cout<<name<<" "<<message[fork[name]].lastproblem<<" "<<lastsituation<<" "<<message[fork[name]].lastt<<"\n";

                        }
                    }else if(status == "Accepted") { //查询 某个队伍 的最后一次状态为 Accepted 的提交
                        //TODO 需要改动 TODO TODO TODO
                        if(message[fork[name]].lastAC == '0') {
                            std :: cout<<"Cannot find any submission.\n";
                        }else {
                            std :: cout<<name<<" "<<message[fork[name]].lastAC<<" "<<"Accepted "<<message[fork[name]].AC[message[fork[name]].lastAC-'A']<<"\n";
                        }

                    }else if(status == "Wrong_Answer") {
                        ////TODO 需要改动 TODO TODO TODO
                        if(message[fork[name]].lastWA == '0') {
                            std :: cout<<"Cannot find any submission.\n";
                        }else {
                            std :: cout<<name<<" "<<message[fork[name]].lastWA<<" "<<"Wrong_Answer "<<message[fork[name]].WA[message[fork[name]].lastWA-'A']<<"\n";
                        }

                    }else if(status == "Runtime_Error") {
                        ////TODO 需要改动 TODO TODO TODO
                        if(message[fork[name]].lastRE == '0') {
                            std :: cout<<"Cannot find any submission.\n";
                        }else {
                            std :: cout<<name<<" "<<message[fork[name]].lastRE<<" "<<"Runtime_Error "<<message[fork[name]].RE[message[fork[name]].lastRE-'A']<<"\n";
                        }

                    }else if(status == "Time_Limit_Exceed") {
                        //TODO 需要改动 TODO TODO TODO
                        if(message[fork[name]].lastTLE == '0') {
                            std :: cout<<"Cannot find any submission.\n";
                        }else {
                            std :: cout<<name<<" "<<message[fork[name]].lastTLE<<" "<<"Time_Limit_Exceed "<<message[fork[name]].TLE[message[fork[name]].lastTLE-'A']<<"\n";
                        }

                    }

                }else { //对指定题目进行查询 EXIST A PROBLEM
                    int serial = problemname[0]-'A';
                    if(status == "ALL") {  //查询某队伍最后一次对题目 A 的提交
                        std :: string now;
                        int consumption;
                        if(message[fork[name]].situation[serial] == 0) {

                            std :: cout<<"Cannot find any submission.\n";

                        }else {
                            if(message[fork[name]].fake[serial] == 1) {

                                now = "Accepted";
                                consumption = message[fork[name]].AC[serial];

                            }else if(message[fork[name]].fake[serial] == 2) {

                                now = "Wrong_Answer";
                                consumption = message[fork[name]].WA[serial];

                            }else if(message[fork[name]].fake[serial] == 3) {

                                now = "Runtime_Error";
                                consumption = message[fork[name]].RE[serial];

                            }else if(message[fork[name]].fake[serial] == 4) {

                                now = "Time_Limit_Exceed";
                                consumption = message[fork[name]].TLE[serial];

                            }
                            std :: cout << name <<" "<< problemname <<" "<< now <<" "<< consumption <<"\n";
                        }

                    }else if(status == "Accepted") { //查询某队最后一次对 M 题状态为 status 的提交
                        if(message[fork[name]].AC[serial] == 0) {

                            std :: cout<<"Cannot find any submission.\n";
                        }else {

                            std :: cout<< name <<" "<< problemname <<" "<< status <<" "<< message[fork[name]].AC[serial] <<"\n";
                        }
                    }else if(status == "Wrong_Answer") {
                        if(message[fork[name]].WA[serial] == 0) {

                            std :: cout<<"Cannot find any submission.\n";
                        }else {

                            std :: cout<< name <<" "<< problemname <<" "<< status <<" "<< message[fork[name]].WA[serial] <<"\n";
                        }
                    }else if(status == "Runtime_Error") {
                        if(message[fork[name]].RE[serial] == 0) {

                            std :: cout<<"Cannot find any submission.\n";
                        }else {

                            std :: cout<< name <<" "<< problemname <<" "<< status <<" "<< message[fork[name]].RE[serial] <<"\n";
                        }
                    }else if(status == "Time_Limit_Exceed") {
                        if(message[fork[name]].TLE[serial] == 0) {

                            std :: cout<<"Cannot find any submission.\n";
                        }else {

                            std :: cout<< name <<" "<< problemname <<" "<< status <<" "<< message[fork[name]].TLE[serial] <<"\n";
                        }
                    }
                }

            }

        }else if(s == "END") {
            std::cout << "[Info]Competition ends.\n";//结束比赛
            break;
        }
    }

    return 0;
}
