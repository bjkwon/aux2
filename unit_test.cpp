//16
//const char cmd[N_COMMANDS][256] = { 
//	"a=[1:3 8:2:25]", 
//	"a(3:5)=90:92", 
//	"a",
//	"a(7:9)=1",
//	"a",
//	"a=[1:3 8:2:25]",
//	"a(3)=[-1 -2 -3]",
//	"a",
//	"a=[1:3 8:2:25]",
//	"a(1:3)=[]",
//	"a",
//	"a=1:10",
//	"a(2:5)=102:105", // contig
//	"a",
//	"a([2 3 5 6]) = 102:105", // not contig
//	"a",
//};

//16
//const char cmd[N_COMMANDS][256] = { 
//	"x=(1:24).group(4)", 
//	"x(4)", 
//	"x(2,3)", 
//	"x(2,3:100)", /*should make an error but not... will crash at some point.. check tree_NARGS-->ExtractByIndex-->extract*/ 
//	"x(4)=-1;",
//	"x",
//	"x(2,3:6)=-2;",
//	"x",
//	"x(2,3:6)=1:4 ;",
//	"x",
//	"x(1:3,2)=[0 0 0]' ;",
//	"x",
//	"x(1:3,2:3)=[9 9; 8 8; 7 7] ;",
//	"x",
//	"x([1 3 4],2:3)=[0 0; 2 2; 6 6] ;",
//	"x",
//};

// 11
//const char cmd[N_COMMANDS][256] = {
//	"x = noise(1000)@ - 40", 
//	"x(10~510)=tone(400,500)@-10 ;", 
//	"y=x(10~510)",
//	"y.rms",
//	"x(10~510)=tone(400,400)@-10 ;",
//	"y=x(10~510)",
//	"y.rms",
//	"y=x(10~410)",
//	"y.rms",
//  "x(10~510)=[] ;",
//  "x.dur",
//};

// 8
//const char cmd[N_COMMANDS][256] = {
//	"x = { 1, \"bjk\",tone(400,1000) }",
//	"x{1}=1:2:20 ;",
//	"x{1}",
//	"x{3}(10~510)=silence(500) ;",
//	"y=x{3}(10~510) ;",
//	"y.rms",
//	"x{1}(3:5)=[0 0 -1] ;",
//	"x{1}",
//};

/* Error
* x = noise(1000)
* x(40~50)=[1 2 3]  //error
* a={1:2:20, noise(1000)}
* a(2)={1:2:20, noise(1000)} //error
* x = [3 2 -1 8 4]
* a = 1:3
* [a(2), y] = max(x) // error  [a.prop y] = max(x) is OK 9/6/2022
* [x,y]=sum(a) //error
*/

//7
//const char cmd[N_COMMANDS][256] = {
//	"a = [3 2 5 8 9]",
//	"[x,y]=max(a)",
//	"x",
//	"y",
//	"[p,q]=temp2",
//	"p",
//	"q",
//};

//5
//const char cmd[N_COMMANDS][256] = {
//	"x = [3 2 5 8 9]",
//	"y = \"merong\" ",
//	"a.val = 777",
//	"a.name = y",
//	"a",
//};

//#define N_COMMANDS 11
//const char cmd[N_COMMANDS][256] = {
//	"x.val(4)=100", // error
//	"x.val{4}=100", // error
//	"x.val(4~5)=100", // error
//	"x=100;",
//	"x.val=200;",
//	"x.name=\"auxlab console\"; ",
//	"x.val = x.val+x;",
//	"x", // ok up to this line
//	"x.name = 999"
//	"x", 
//	"x.name(2) = noise(40) " // error
//};

//a = sin((1:10)/5*$pi)
//a(a>0)=.88
//a(a>0)=a(a>0)*2
//a(a>0)=[]
//12:08 AM 10/9/2022=====================
//This shouldn't make an error
//AUX> a(a>0)=[]
//Error: a(range)=[] -- range should be consecutive. line 1, col 3
//12:08 AM 10/9/2022======================
//To do
//a(a>0)=..*2
//a(a>0)*=2
//
//a=[3 5 -1 7 9 -22 7 2 6 0 5]
//k=1:a.length
//a(k%2==0)
//a(a%2==0)
//a(a%2==0) = a(a%2==0)*10
//To do
//a(a%2==0) *= 10
//a(a%2==0) = .. * 10
//
//To do
//"end" indexing reserved var
//To do 
//"_" indexing reserved var

a=5
a=mean(..) //RL-N 
a=10:10:50
a(3:5)=mean(..) //RL-X
x=noise(5000)
x(20~30)=..@-20 //RL-T

mm.vals=[4 8 5 7 -1]
mm.tag="testcase"
mm(2)=2 // error
mm.vals(3:5)= [ 1 3 -10]
mm.vals(3:5)= mean(..) // RL-S RL-X

x=noise(100).ramp(10)@-10
x@=-20
x.rms==-20 // this doesn't work with float precision because x.rms is something like -20.000005
// But abs(x.rms+20) < 0.0001 didn't work because x.rms+20 crashed   11:19 PM 10/11/2022
// just check with x.rms
x@@=5
x.rms==-15

x = { 1, [4 8 5 7 -1],tone(400,1000) }
x=...length
x = { 1, [4 8 5 7 -1],tone(400,1000) }
x{1}*=10
x{2}(3:5)= mean(..)
x{3}=...rms
x = { 1, [4 8 5 7 -1],tone(400,1000) }
x{3}(100~200)@=-20
y1=x{3}(0~100)

x=noise(100)
x(20~30)=..@-20
x.group(10).rms // DIDN'T WORK 12:15 AM 10/12/2022

// WORKING 05:09 PM 10/12/2022
x=noise(100)+noise(100).ramp(10)@-20>>500
x(10~20)@=-20
x.group(10).rms

x=noise(10)+noise(10).ramp(3)@-20>>50
x(5~55)@=-10

x=noise(100)+noise(100).ramp(30)@-20>>500
x(50~550)@=-40
//CHECKED 1:05 AM 10/13/2022