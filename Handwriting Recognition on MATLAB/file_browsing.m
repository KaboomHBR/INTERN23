% 声明文件名及路径的全局变量以供调用。
% Declare the global variables for the file name and its path.
global file_name;
global path_name;

% This function file allows user to browse and choose image files.
[file_name, path_name]=uigetfile({'*.jpg';'*.png'},'Choose the image file'); % 花括号内是文件后缀名，目前支持jpg和png两种格式的图片。引号内是弹窗标题。
f=[path_name, file_name];
original_image=imread(f);
axes(ax_1); % 这一行是为了确保图片显示在坐标区1而不是其他坐标区。
imshow(original_image)
title('Original image'); % 图片显示后的标题，可以修改。


