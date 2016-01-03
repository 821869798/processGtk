#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"


GtkWidget *clist;
GtkWidget *addWindow = NULL; //添加进程的窗口
// 输入框
GtkWidget *nameEntry,*arriveHourEntry,*arriveMinuteEntry,*workTimeEntry,*priorityEntry;


GtkWidget * createMainWindow(); //创建主窗口
GtkWidget * createAddProcessWindow(); //创建添加进程窗口
GtkWidget *createResultWindow(process **datas,int count,int type); //显示调度结果的窗口
void addProcess_clicked(GtkButton * button,gpointer data);  //添加进程的按钮事件
void deleteProcess_clicked(GtkButton * button,gpointer data);  //删除进程的按钮事件
void clearProcess_clicked(GtkButton * button,gpointer data);  //删除进程的按钮事件
void showResult_clicked(GtkButton * button,gpointer data); //显示进程调度算法结果的按钮事件
void addSubmit_clicked(GtkButton *button,gpointer data); //提交添加的程的按钮事件
void addCancel_clicked(); //取消添加进程的按钮事件
void addProcess(char *name,char *arriveHour,char *arriveMinute,char *workTime,char *priority); //添加进程


static void on_entry_insert_text(GtkEditable *editable,gchar *new_text,gint new_text_length,gint *position,gpointer user_data); //限制输入框只能输入数字

int main()
{
    GtkWidget *window;
    gtk_init(0,NULL);
    window = createMainWindow();
    gtk_widget_show_all(GTK_WINDOW(window));
    gtk_main();
    return 0;
}


GtkWidget * createMainWindow()
{
    GtkWidget *button;
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
    gtk_window_set_title(GTK_WINDOW(window),"主窗口");
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window),5);
    //gtk_window_set_default_size(GTK_WINDOW(window),500,400);
    GtkWidget *vbox,*hbox,*hbox1;
    vbox = gtk_vbox_new(FALSE,0);
    hbox = gtk_hbox_new(FALSE,0);
    hbox1 = gtk_hbox_new(FALSE,0);
    gtk_container_add(GTK_CONTAINER(window),vbox);
    gchar *titles[] = {"进程名","到达时间(小时位)","到达时间(分钟位)","运行时间","优先级"};
    clist = gtk_clist_new_with_titles(5,titles);
//    gchar *data1[] = {"j2","8","30","50","2"};
//    gchar *data2[] = {"j4","8","50","10","3"};
//    gchar *data3[] = {"j1","8","0","60","1"};
//    gchar *data4[] = {"j3","8","40","30","4"};
//    gtk_clist_append(GTK_CLIST(clist),data1);
//    gtk_clist_append(GTK_CLIST(clist),data2);
//    gtk_clist_append(GTK_CLIST(clist),data3);
//    gtk_clist_append(GTK_CLIST(clist),data4);
    gtk_clist_set_column_width(GTK_CLIST(clist),0,50);
    gtk_clist_set_column_width(GTK_CLIST(clist),1,120);
    gtk_clist_set_column_width(GTK_CLIST(clist),2,120);
    gtk_clist_set_column_width(GTK_CLIST(clist),3,70);
    gtk_clist_set_column_width(GTK_CLIST(clist),4,50);
    gtk_clist_set_selection_mode(GTK_CLIST(clist),GTK_SELECTION_SINGLE);
    gtk_box_pack_start(GTK_BOX(vbox),clist,TRUE,TRUE,0);

    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,TRUE,5);
    button = gtk_button_new_with_label("添加进程");
    gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,5);
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(addProcess_clicked),NULL);
    button = gtk_button_new_with_label("删除选中进程");
    gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,5);
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(deleteProcess_clicked),NULL);
    button = gtk_button_new_with_label("清空");
    gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,5);
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(clearProcess_clicked),NULL);

    gtk_box_pack_start(GTK_BOX(vbox),hbox1,FALSE,TRUE,5);
    button = gtk_button_new_with_label("先来先服务");
    gtk_box_pack_start(GTK_BOX(hbox1),button,TRUE,TRUE,5);
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(showResult_clicked),(gpointer)0);
    button = gtk_button_new_with_label("最短作业优先");
    gtk_box_pack_start(GTK_BOX(hbox1),button,TRUE,TRUE,5);
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(showResult_clicked),(gpointer)1);
    button = gtk_button_new_with_label("最短剩余时间优先");
    gtk_box_pack_start(GTK_BOX(hbox1),button,TRUE,TRUE,5);
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(showResult_clicked),(gpointer)2);
    button = gtk_button_new_with_label("非抢占式优先级");
    gtk_box_pack_start(GTK_BOX(hbox1),button,TRUE,TRUE,5);
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(showResult_clicked),(gpointer)3);
    button = gtk_button_new_with_label("抢占式优先级");
    gtk_box_pack_start(GTK_BOX(hbox1),button,TRUE,TRUE,5);
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(showResult_clicked),(gpointer)4);
    return window;
}

GtkWidget * createAddProcessWindow()
{
    GtkWidget *window;
    GtkWidget *label;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(addCancel_clicked),(gpointer)2);
    gtk_window_set_title(GTK_WINDOW(window),"添加进程窗口");
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window),5);
    GtkWidget *vbox,*hbox;
    vbox = gtk_vbox_new(FALSE,0);
    gtk_container_add(GTK_CONTAINER(window),vbox);

    hbox = gtk_hbox_new(FALSE,0);
    label = gtk_label_new("进程名:");
    nameEntry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox),label,TRUE,TRUE,5);
    gtk_box_pack_start(GTK_BOX(hbox),nameEntry,TRUE,TRUE,5);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);

    hbox = gtk_hbox_new(FALSE,0);
    label = gtk_label_new("到达时间(小时位):");
    gtk_box_pack_start(GTK_BOX(hbox),label,TRUE,TRUE,5);
    arriveHourEntry = gtk_entry_new();
    g_signal_connect(G_OBJECT(arriveHourEntry), "insert-text", G_CALLBACK(on_entry_insert_text), NULL);
    gtk_box_pack_start(GTK_BOX(hbox),arriveHourEntry,TRUE,TRUE,5);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);

    hbox = gtk_hbox_new(FALSE,0);
    label = gtk_label_new("到达时间(分钟位):");
    gtk_box_pack_start(GTK_BOX(hbox),label,TRUE,TRUE,5);
    arriveMinuteEntry = gtk_entry_new();
    g_signal_connect(G_OBJECT(arriveMinuteEntry), "insert-text", G_CALLBACK(on_entry_insert_text), NULL);
    gtk_box_pack_start(GTK_BOX(hbox),arriveMinuteEntry,TRUE,TRUE,5);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);

    hbox = gtk_hbox_new(FALSE,0);
    label = gtk_label_new("运行时间(分钟):");
    gtk_box_pack_start(GTK_BOX(hbox),label,TRUE,TRUE,5);
    workTimeEntry = gtk_entry_new();
    g_signal_connect(G_OBJECT(workTimeEntry), "insert-text", G_CALLBACK(on_entry_insert_text), NULL);
    gtk_box_pack_start(GTK_BOX(hbox),workTimeEntry,TRUE,TRUE,5);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);

    hbox = gtk_hbox_new(FALSE,0);
    label = gtk_label_new("优先级(大的优先):");
    gtk_box_pack_start(GTK_BOX(hbox),label,TRUE,TRUE,5);
    priorityEntry = gtk_entry_new();
    //gtk_entry_set_text(GTK_ENTRY(priorityEntry),"0");
    g_signal_connect(G_OBJECT(priorityEntry), "insert-text", G_CALLBACK(on_entry_insert_text), NULL);
    gtk_box_pack_start(GTK_BOX(hbox),priorityEntry,TRUE,TRUE,5);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);

    hbox = gtk_hbox_new(FALSE,0);
    GtkWidget *button;
    button = gtk_button_new_with_label("添加");
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(addSubmit_clicked),(gpointer)1);
    gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,5);
    gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);
    return window;
}

GtkWidget *createResultWindow(process **datas,int count,int type)
{
    char *windowTitle[20];
    switch (type) {
    case 0:
        strcpy(windowTitle,"先来先服务");
        break;
    case 1:
        strcpy(windowTitle,"最短作业优先");
        break;
    case 2:
        strcpy(windowTitle,"最短剩余时间优先");
        break;
    case 3:
        strcpy(windowTitle,"非抢占式优先级");
        break;
    default:
        strcpy(windowTitle,"抢占式优先级");
        break;
    }
    GtkWidget *resultClist;
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),windowTitle);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window),5);
    GtkWidget *vbox;
    vbox = gtk_vbox_new(FALSE,0);
    gtk_container_add(GTK_CONTAINER(window),vbox);
    if(type==3||type==4)
    {
        gchar *titles[] = {"进程名","到达时间","运行时间","开始时间","结束时间","周转时间","带权周转时间","优先级"};
        resultClist = gtk_clist_new_with_titles(8,titles);
        gtk_clist_set_column_width(GTK_CLIST(resultClist),0,50);
        gtk_clist_set_column_width(GTK_CLIST(resultClist),1,100);
        gtk_clist_set_column_width(GTK_CLIST(resultClist),2,80);
        gtk_clist_set_column_width(GTK_CLIST(resultClist),3,100);
        gtk_clist_set_column_width(GTK_CLIST(resultClist),4,100);
        gtk_clist_set_column_width(GTK_CLIST(resultClist),5,80);
        gtk_clist_set_column_width(GTK_CLIST(resultClist),6,120);
        gtk_clist_set_column_width(GTK_CLIST(resultClist),7,80);
        gtk_box_pack_start(GTK_BOX(vbox),resultClist,TRUE,TRUE,0);
    }
    else
    {
        gchar *titles[] = {"进程名","到达时间","运行时间","开始时间","结束时间","周转时间","带权周转时间"};
        resultClist = gtk_clist_new_with_titles(7,titles);
        gtk_clist_set_column_width(GTK_CLIST(resultClist),0,50);
        gtk_clist_set_column_width(GTK_CLIST(resultClist),1,100);
        gtk_clist_set_column_width(GTK_CLIST(resultClist),2,80);
        gtk_clist_set_column_width(GTK_CLIST(resultClist),3,100);
        gtk_clist_set_column_width(GTK_CLIST(resultClist),4,100);
        gtk_clist_set_column_width(GTK_CLIST(resultClist),5,80);
        gtk_clist_set_column_width(GTK_CLIST(resultClist),6,120);
        gtk_box_pack_start(GTK_BOX(vbox),resultClist,TRUE,TRUE,0);
    }
    int i;

    for(i=0;i<count;i++)
    {
        char arriveTimeStr[20];
        ptimeToStr(datas[i]->arriveTime,arriveTimeStr);
        char workTimeStr[20];
        sprintf(workTimeStr,"%d",datas[i]->workTime);
        char startTimeStr[20];
        ptimeToStr(datas[i]->startTime,startTimeStr);
        char endTimeStr[20];
        ptimeToStr(datas[i]->endTime,endTimeStr);
        char turnaroundTimeStr[20];
        sprintf(turnaroundTimeStr,"%d",datas[i]->turnaroundTime);
        char turnaroundRatioStr[20];
        sprintf(turnaroundRatioStr,"%g",datas[i]->turnaroundRatio);
        if(type==3||type==4)
        {
            char priorityStr[20];
            sprintf(priorityStr,"%d",datas[i]->priority);
            char *dataRow[] = {datas[i]->name,arriveTimeStr,workTimeStr,startTimeStr,
                               endTimeStr,turnaroundTimeStr,turnaroundRatioStr,priorityStr};
            gtk_clist_append(GTK_CLIST(resultClist),dataRow);
        }
        else
        {

            char *dataRow[] = {datas[i]->name,arriveTimeStr,workTimeStr,startTimeStr,
                               endTimeStr,turnaroundTimeStr,turnaroundRatioStr};
            gtk_clist_append(GTK_CLIST(resultClist),dataRow);
        }
    }

    return window;
}

void addProcess_clicked(GtkButton * button,gpointer data)
{
    if(addWindow==NULL)
    {
        addWindow = createAddProcessWindow();
        gtk_widget_show_all(addWindow);
    }
}

void deleteProcess_clicked(GtkButton * button,gpointer data)
{
    gtk_clist_remove(GTK_CLIST(clist),GTK_CLIST(clist)->focus_row);
}

void clearProcess_clicked(GtkButton * button,gpointer data)
{
     gtk_clist_clear(GTK_CLIST(clist));
}

void addSubmit_clicked(GtkButton *button,gpointer data)
{

    char * name = gtk_entry_get_text(GTK_ENTRY(nameEntry));
    char *arriveHourStr = gtk_entry_get_text(GTK_ENTRY(arriveHourEntry));
    char *arriveMinuteStr = gtk_entry_get_text(GTK_ENTRY(arriveMinuteEntry));
    char *workTimeStr = gtk_entry_get_text(GTK_ENTRY(workTimeEntry));
    char *priorityStr = gtk_entry_get_text(GTK_ENTRY(priorityEntry));
    if(!strcmp(name,"")||strlen(name)>10||!strcmp(arriveHourStr,"")||!strcmp(arriveMinuteStr,"")||
            !strcmp(workTimeStr,"")||!strcmp(priorityStr,"")||atoi(arriveMinuteStr)>=60||atoi(workTimeStr)==0)
    {
        GtkWidget *dialog;
        dialog = gtk_message_dialog_new(NULL,
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_INFO,
                                        GTK_BUTTONS_OK,
                                        "输入的数据不合法");
        //gtk_window_set_title(GTK_WINDOW(dialog), "Information");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    addProcess(name,arriveHourStr,arriveMinuteStr,workTimeStr,priorityStr);
    gtk_widget_destroy(addWindow);
    addWindow = NULL;
}

void addCancel_clicked()
{
    gtk_widget_destroy(addWindow);
    addWindow = NULL;
}

void addProcess(char *name,char *arriveHour,char *arriveMinute,char *workTime,char *priority)
{
    char *tmpStr[]={name,arriveHour,arriveMinute,workTime,priority};
    gtk_clist_append(GTK_CLIST(clist),tmpStr);
}

void showResult_clicked(GtkButton * button,gpointer data)
{
    int type = GPOINTER_TO_INT(data);
    if(GTK_CLIST(clist)->rows==0)
    {
        GtkWidget *dialog;
        dialog = gtk_message_dialog_new(NULL,
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_INFO,
                                        GTK_BUTTONS_OK,
                                        "还没有录入进程,不能进行调度");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    int i;
    int count = GTK_CLIST(clist)->rows;
    process **processDatas = (process**)malloc(sizeof(process*)*count);
    for(i=0;i<count;i++)
    {
        processDatas[i] = (process*)malloc(sizeof(process));
        char *tmpStr;
        gtk_clist_get_text(GTK_CLIST(clist),i,0,&tmpStr);
        strcpy(processDatas[i]->name,tmpStr);
        gtk_clist_get_text(GTK_CLIST(clist),i,1,&tmpStr);
        processDatas[i]->arriveTime.hour = atoi(tmpStr);
        gtk_clist_get_text(GTK_CLIST(clist),i,2,&tmpStr);
        processDatas[i]->arriveTime.min = atoi(tmpStr);
        gtk_clist_get_text(GTK_CLIST(clist),i,3,&tmpStr);
        processDatas[i]->workTime = atoi(tmpStr);
        gtk_clist_get_text(GTK_CLIST(clist),i,4,&tmpStr);
        processDatas[i]->priority = atoi(tmpStr);
    }
    if(type==0)
    {
        FCFS(processDatas,count);
        GtkWidget *window = createResultWindow(processDatas,count,0);
        gtk_widget_show_all(window);
    }
    else if(type==1)
    {
        SJF(processDatas,count);
        GtkWidget *window = createResultWindow(processDatas,count,1);
        gtk_widget_show_all(window);
    }
    else if(type==2)
    {
        SRTF(processDatas,count);
        GtkWidget *window = createResultWindow(processDatas,count,2);
        gtk_widget_show_all(window);
    }
    else if(type==3)
    {
        NPP(processDatas,count);
        GtkWidget *window = createResultWindow(processDatas,count,3);
        gtk_widget_show_all(window);
    }
    else
    {
        PP(processDatas,count);
        GtkWidget *window = createResultWindow(processDatas,count,4);
        gtk_widget_show_all(window);
    }

}
void on_entry_insert_text(GtkEditable *editable,gchar *new_text,gint new_text_length,gint *position,gpointer user_data)
{
    const char *text = gtk_entry_get_text( GTK_ENTRY(editable) );

    if( !strcmp(new_text, "0") ||
            !strcmp(new_text, "1") ||
            !strcmp(new_text, "2") ||
            !strcmp(new_text, "3") ||
            !strcmp(new_text, "4") ||
            !strcmp(new_text, "5") ||
            !strcmp(new_text, "6") ||
            !strcmp(new_text, "7") ||
            !strcmp(new_text, "8") ||
            !strcmp(new_text, "9")
            )
    {
        return;
    }
    else
    {
        gchar *result = g_utf8_strup (new_text, new_text_length);
        g_signal_handlers_block_by_func (editable,
                                         (gpointer) on_entry_insert_text, user_data);
        gtk_entry_set_text (GTK_ENTRY(editable), text);
        g_signal_handlers_unblock_by_func (editable,
                                           (gpointer) on_entry_insert_text, user_data);
        g_signal_stop_emission_by_name (editable, "insert_text");
        g_free (result);
    }
}

//#include <gtk/gtk.h>
//#include <stdlib.h>
//static GtkWidget *entry;
//gint count = 0;
//gdouble nn = 0;
//gdouble mm = 0;
//gint s = 0;
//gboolean first = TRUE; //首次输入
//gboolean have_dot = FALSE; //是否有小数点
//gboolean have_result = FALSE; //是否有结果输出
//gchar number[100]; //保存用户输入的数字

//void clear_all()
//{
//    gint i;
//    gtk_entry_set_text(GTK_ENTRY(entry),"");
//    nn = 0;
//    mm = 0;
//    s = 0;
//    count = 0;
//    first = TRUE;
//    have_dot = FALSE;
//    have_result = FALSE;
//    for(i=0;i<100;i++)
//        number[i] = 0;
//}

//void on_num_clicked(GtkButton * button,gpointer data)
//{
//    const gchar * num;
//    gint i;
//    if(have_result)
//        clear_all();
//    if(count == 6) return;
//    count++;
//    num = gtk_button_get_label(GTK_BUTTON(button));
//    i = g_strlcat(number,num,100);
//    if(first)
//        nn = strtod(number,NULL);
//    else
//        mm = strtod(number,NULL);
//    gtk_entry_set_text(GTK_ENTRY(entry),number);
//}

//void on_dot_clicked(GtkButton * button,gpointer data)
//{
//    gint i;
//    if(have_result)
//        clear_all();//
//    if(have_dot == FALSE) //
//    {
//        have_dot = TRUE;
//        i = g_strlcat(number,".",100);
//        gtk_entry_set_text(GTK_ENTRY(entry),number);
//    }
//}

//void on_clear_clicked(GtkButton * button,gpointer data)
//{
//    clear_all();
//}

//void on_suan_clicked(GtkButton * button,gpointer data)
//{
//    gint i;
//    switch(GPOINTER_TO_INT(data))
//    {
//    case 1: //"+"
//        s = 1;
//        gtk_entry_set_text(GTK_ENTRY(entry),"");
//        first = FALSE ; count = 0; break;
//    case 2: //"-"
//        s = 2;
//        gtk_entry_set_text(GTK_ENTRY(entry),"");
//        first = FALSE ; count = 0; break;
//    case 3: //"*"
//        s = 3;
//        gtk_entry_set_text(GTK_ENTRY(entry),"");
//        first = FALSE ; count = 0; break;
//    case 4: //"/"
//        s = 4;
//        gtk_entry_set_text(GTK_ENTRY(entry),"");
//        first = FALSE ; count = 0; break;
//    }
//    have_dot = FALSE;
//    for(i = 0 ; i < 100 ; i++) //
//        number[i] = '\0';
//}

//void on_eq_clicked(GtkButton * button, gpointer data)
//{
//    double numb;
//    gchar num[100];
//    gchar *result;
//    switch(s)
//    {
//    case 1: //"+"
//        numb = nn+mm;
//        break;
//    case 2: //"-"
//        numb = nn-mm;
//        break;
//    case 3: //"*"
//        numb = nn*mm;
//        break;
//    case 4: //"/"
//        numb = nn/mm;
//        break;
//    }
//    result = g_ascii_dtostr(num,100,numb);
//    gtk_entry_set_text(GTK_ENTRY(entry),result);
//    have_result = TRUE;
//}

//int main(int argc,char *argv[])
//{
//    GtkWidget *window;
//    GtkWidget *vbox;
//    GtkWidget *hbox,*hbox1,*hbox2,*hbox3,*hbox4;
//    GtkWidget *button;
//    GtkWidget *label;
//    gtk_init(&argc,&argv);
//    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//    g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
//    gtk_window_set_title(GTK_WINDOW(window),"计算器");
//    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
//    gtk_container_set_border_width(GTK_CONTAINER(window),5);
//    vbox = gtk_vbox_new(FALSE,0);
//    gtk_container_add(GTK_CONTAINER(window),vbox);
//    hbox = gtk_hbox_new(FALSE,0);
//    gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,5);
//    label = gtk_label_new("Calculator");
//    gtk_box_pack_start(GTK_BOX(hbox),label,TRUE,TRUE,5);
//    button = gtk_button_new_with_label("C");
//    gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,5);
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_clear_clicked),NULL);
//    entry = gtk_entry_new();
//    gtk_widget_set_direction(entry,GTK_TEXT_DIR_RTL);
//    gtk_box_pack_start(GTK_BOX(vbox),entry,FALSE,FALSE,5);

//    hbox1 = gtk_hbox_new(FALSE,0);
//    gtk_box_pack_start(GTK_BOX(vbox),hbox1,TRUE,TRUE,5);
//    button = gtk_button_new_with_label("3");
//    gtk_box_pack_start(GTK_BOX(hbox1),button,TRUE,TRUE,5);
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_num_clicked),NULL);
//    button = gtk_button_new_with_label("2");
//    gtk_box_pack_start(GTK_BOX(hbox1),button,TRUE,TRUE,5);
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_num_clicked),NULL);
//    button = gtk_button_new_with_label("1");
//    gtk_box_pack_start(GTK_BOX(hbox1),button,TRUE,TRUE,5);
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_num_clicked),NULL);
//    button = gtk_button_new_with_label("+");
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_suan_clicked),(gpointer)1);
//    gtk_box_pack_start(GTK_BOX(hbox1),button,TRUE,TRUE,5);
//    hbox2 = gtk_hbox_new(FALSE,0);
//    gtk_box_pack_start(GTK_BOX(vbox),hbox2,TRUE,TRUE,5);
//    button = gtk_button_new_with_label("6");
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_num_clicked),NULL);
//    gtk_box_pack_start(GTK_BOX(hbox2),button,TRUE,TRUE,5);
//    button = gtk_button_new_with_label("5");
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_num_clicked),NULL);
//    gtk_box_pack_start(GTK_BOX(hbox2),button,TRUE,TRUE,5);
//    button = gtk_button_new_with_label("4");
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_num_clicked),NULL);
//    gtk_box_pack_start(GTK_BOX(hbox2),button,TRUE,TRUE,5);
//    button = gtk_button_new_with_label("-");
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_suan_clicked),(gpointer)2);
//    gtk_box_pack_start(GTK_BOX(hbox2),button,TRUE,TRUE,5);
//    hbox3 = gtk_hbox_new(FALSE,0);
//    gtk_box_pack_start(GTK_BOX(vbox),hbox3,TRUE,TRUE,5);
//    button = gtk_button_new_with_label("9");
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_num_clicked),NULL);
//    gtk_box_pack_start(GTK_BOX(hbox3),button,TRUE,TRUE,5);
//    button = gtk_button_new_with_label("8");
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_num_clicked),NULL);
//    gtk_box_pack_start(GTK_BOX(hbox3),button,TRUE,TRUE,5);
//    button = gtk_button_new_with_label("7");
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_num_clicked),NULL);
//    gtk_box_pack_start(GTK_BOX(hbox3),button,TRUE,TRUE,5);
//    button = gtk_button_new_with_label("*");
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_suan_clicked),(gpointer)3);
//    gtk_box_pack_start(GTK_BOX(hbox3),button,TRUE,TRUE,5);
//    hbox4 = gtk_hbox_new(FALSE,0);
//    gtk_box_pack_start(GTK_BOX(vbox),hbox4,TRUE,TRUE,5);
//    button = gtk_button_new_with_label("0");
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_num_clicked),NULL);
//    gtk_box_pack_start(GTK_BOX(hbox4),button,TRUE,TRUE,5);
//    button = gtk_button_new_with_label(".");
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_dot_clicked),NULL);
//    gtk_box_pack_start(GTK_BOX(hbox4),button,TRUE,TRUE,5);
//    button = gtk_button_new_with_label("=");
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_eq_clicked),NULL);
//    gtk_box_pack_start(GTK_BOX(hbox4),button,TRUE,TRUE,5);
//    button = gtk_button_new_with_label("/");
//    g_signal_connect(G_OBJECT(button),"clicked",
//                     G_CALLBACK(on_suan_clicked),(gpointer)4);
//    gtk_box_pack_start(GTK_BOX(hbox4),button,TRUE,TRUE,5);

//    gtk_widget_show_all(window);
//    gtk_main();



//    return 0;
//}
