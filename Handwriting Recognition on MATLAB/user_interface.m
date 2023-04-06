clc;
clear;

% 该部分创建文件名和路径的全局函数以供调用。

% Create global variables for the file name and its path.
global file_name;
global path_name;

% 该部分为用户界面的框架。附带可以直接使用的英文注释（后同）。
% position后面方括号内的参数分别是水平距离（左起），垂直距离（下起），宽度，高度。
% name后面的单引号内是用户界面的标题。
% color后面的方括号内的参数分别是背景色的R，G，B。
% 上述参数可以根据需求随意调整。

% Create the frame of user interface.
interface= figure('menubar','none','resize','off','position',[320 130 900 600],...
'name','Handwriting recognition','color',[0.7 0.7 0.7],'numberTitle','off','visible','on');



% 该部分创建两个坐标区用于分别显示原图和处理后的图片。
% position后面方括号内的参数分别是水平距离（左起），垂直距离（下起），宽度，高度。可以根据需求随意调整。

% Create axes to show the original image.
ax_1=axes('position',[0.4 0.4 0.5 0.5]);



% 该部分创建一个文本框用于输出识别结果。
% text_output用于创建文本框。
% lable_output用于给文本框加标题。
% position后面方括号内的参数分别是水平距离（左起），垂直距离（下起），宽度，高度。
% string后面的单引号内是文本框的标题。
% backgroundcolor后面的方括号内的参数分别是标题背景色的R，G，B。
% fontsize后面为string的字号大小。
% 上述参数可以根据需求随意调整。

% Create a text box to show the result of recognition and give the text box a label. 
global text_output;
text_output=uicontrol('style','edit','position',[50 50 800 120]);
lable_output=uicontrol('style','text','position',[370 20 180 20],'string','Result',...
    'backgroundcolor',[0.7 0.7 0.7],'fontsize',12);



% 该部分创建按钮以供用户浏览文件并选择输入的图片。
% position后面方括号内的参数分别是水平距离（左起），垂直距离（下起），宽度，高度。
% string后面的单引号内是按钮的名字。
% Call后面的单引号内是调用的m文件的文件名。该文件我已写好并一并附上。
% fontsize后面为string的字号大小。
% 上述参数可以根据需求随意调整。

% Create a button for browsing and chooseing the input image file.
button_1= uicontrol('string','Browse the Input Image File','Call','file_browsing',...
    'position',[35 400 275 40],'fontsize',12);



% 该部分创建按钮用于预处理及识别边界无重叠的文字。
% position后面方括号内的参数分别是水平距离（左起），垂直距离（下起），宽度，高度。
% string后面的单引号内是按钮的名字。
% Call后面的单引号内是调用的m文件的文件名。该文件需要写好预处理部分的文件后填入文件名。
% fontsize后面为string的字号大小。
% 上述参数可以根据需求随意调整。

% Pre-processing and recognition for standard characters.
button_2= uicontrol('string','Standard Character','Call','recognition',...
    'position',[190 300 120 30],'fontsize',9);



% 该部分创建按钮用于预处理及识别边界重叠的文字。
% position后面方括号内的参数分别是水平距离（左起），垂直距离（下起），宽度，高度。
% string后面的单引号内是按钮的名字。
% Call后面的单引号内是调用的m文件的文件名。该文件需要写好识别部分的文件后填入文件名。
% fontsize后面为string的字号大小。
% 上述参数可以根据需求随意调整。

% Pre-processing and recognition for overlap characters.
button_3= uicontrol('string','Overlap Character','Call','overlap',...
    'position',[190 240 120 30],'fontsize',9);

