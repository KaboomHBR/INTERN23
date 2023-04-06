function [fl,re] = lines(im_texto)
% this function would divide input images into its first character line and the rest
im_texto = clip_line(im_texto); % 把图像四周的空白干掉
num_filas = size(im_texto,1);%num_filas will be assigned as the num. of rows of the input image, i.e. 纵长
for s = 1:num_filas
    if sum(im_texto(s,:)) == size(im_texto, 2) % if the entire row is white
        fm = im_texto((1:s-1), :); % First line matrix
        rm = im_texto((s:end), :); % Remain line matrix
        fl = clip_line(fm); % 这里的clip，不会改变纵长(number of rows), 但可以改变横长(number of columns)，作用依然是把字母周围的空白干掉
        re=clip_line(rm);
        break %我们只需要提取第一行，所以在这里break，结束for loop
    else
        fl = im_texto;
        re = [ ];
    end % after this end, s+=1, then loop again
end


function img_out=clip_line(img_in)
[f, c]=find(~img_in); %f and c are n*1 vectors, where n is the number of elements in img_in that is zero (img_in is binary data, and zero==black, one==white)
img_out=img_in(min(f):max(f),min(c):max(c));%Crops image