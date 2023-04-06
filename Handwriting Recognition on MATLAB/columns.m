function [fl,rec] = columns(im_texto)
% this function would divide the input image (consists of a line of characters) into its first character and the rest
im_texto = clip_column(im_texto); % 把图像四周的空白干掉
num_filas = size(im_texto,2);%num_filas will be assigned as the num. of columns of the input image, i.e. 横长
for s = 1:num_filas
    if sum(im_texto(:,s)) == size(im_texto, 1) % if the entire column is white 这里还可以改！！加个threashold来实现连笔识别
        fm = im_texto(:, (1:s-1)); % First column matrix
        rm = im_texto(:, (s:end)); % Remain column matrix
        fl = clip_column(fm); % 这里的clip，不会改变横长(number of columns), 但可以改变纵长(number of rows)，作用依然是把字母周围的空白干掉
        rec = clip_column(rm);
        break
    else
        fl = im_texto;
        rec = [ ];
    end % after this end, s+=1, then loop again
end

function img_out=clip_column(img_in)
[f, c]=find(~img_in); %f and c are n*1 vectors, where n is the number of elements in img_in that is zero (img_in is binary data, and zero==black, one==white)
img_out=img_in(min(f):max(f),min(c):max(c));%Crops image