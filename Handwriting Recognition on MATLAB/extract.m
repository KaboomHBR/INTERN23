function extract(is_italic)
global text_output; % Declare the global variable for the output area.
global file_name;
global path_name;
% extract letter images and identify them
% if is_italic is 1, meaning we are dealing with 
letter=[path_name,file_name];
imagen = imread(letter); % input image
% by default the input image is a binary image
% so these lines below could be deleted later
% Convert to gray scale
if size(imagen,3)==3 %RGB image
    imagen=rgb2gray(imagen);
end
% Convert to BW
if ~islogical(imagen(1,1))
    imagen = imadjust(imagen); %higher the contrast
    imagen = imbinarize(imagen);
end
H = fspecial("average",3);
imagen = imfilter(imagen,H,"replicate");
%imagen = imbinarize(gssmooth);
% Remove all object containing fewer than 15 pixels
% just a way of filtering
imagen = bwareaopen(imagen,15);

% show image
imshow(imagen);
title('Input Image after Filtered')
% output matrix that could be written into the txt file
word = [ ];
re = imagen;
fid = fopen('text.txt', 'wt');
folder_img = 'img';
folder_img_noresize = 'img_noresize';
% load the neural network, named trainedNetwork_1
load('sixth_try_net.mat');
net = trainedNetwork_1;
counter=1;
while 1
    % Fcn 'lines' seperate lines in text
    [fl, re] = lines(re); % fl->first line; re->remain line
    imgn = fl;
    is_italic_loop = is_italic;
    while 1
        if is_italic_loop==0
            [fc, rec] = columns(imgn); %fc->first column; rec->remain column; actually here its just the first character in line imgn
            % resize fc, a picture containing the first character
            img_letter = advanced_resize(fc);
            imgn = rec;
            Ne = 0;
        else
            [L, Ne] = bwlabel(~imgn); % 关于L：是一个double矩阵，和imgn一样大，但内容不再只有0、1。以一行HEAVY为例，H的位置会标注为1，E标注为2，以此类推。。。
            [r, c] = find(L==is_italic_loop);
            fc=imgn(min(r):max(r),min(c):max(c));
            img_letter = advanced_resize(fc);
            is_italic_loop = is_italic_loop + 1;
            rec = logical([1]);
        end
        imwrite(fc,[folder_img_noresize,num2str(counter),'.png']);
        imwrite(img_letter,[folder_img,num2str(counter),'.png']);
        imwrite(img_letter,'letter.png'); % store the image so that imageDatastore() could be used
        img = imageDatastore('letter.png');
        % use neural network to recognize the letter
        Netread = classify(net, img); % Netread is a categorical object, cannot output directly
        letter = char(Netread); % convert it into char. string and store in variable 'letter'
        letter = smart_recon(letter, word, is_italic); % convert the character string with categories into actual English characters, e.g. from 'As' (lowercase A) to 'a'
        % store this letter into the output matrix 'word'
        word = [word, letter];
        % imgn = rec;
        counter=counter+1;
        % when a line finished, breaks the loop
        if (isempty(rec) || is_italic_loop>Ne)
            break
        end
    end
    % write this line into file
    fprintf(fid, '%c', word);
    fprintf(fid, '\n');
    % clear 'word' so that could move on to the next line
    word = [ ];
    counter=counter+1;
    % when all the lines are read, breaks the loop
    if isempty(re)
        fclose(fid);
        break
    end
end
%data=importdata('text.txt');
fid = fopen('text.txt');
data = fscanf(fid, '%c');
set(text_output,'max',100);
set(text_output,'string',data,'horizontalalignment','left','fontsize',12); % Output the string in the result area.
delete('*.png','*jpg');
fclose(fid);
end

function img_output = advanced_resize(fc)
% recize input img into [128, 128] without changing the aspect ratio
    [r, c] = size(fc);
    if r>=c
        cc = round((r-c)/2); % num of columns each side has to expand
        new_one = ones(r, cc);
        new_one = logical(new_one);
        fc = [new_one fc new_one];
    else
        rr = round((c-r)/2);
        new_one = ones(rr, c);
        new_one = logical(new_one);
        fc = [new_one; fc; new_one];
    end
    img_output = imresize(fc, [128, 128]);
end
function correctletter = smart_recon(letter, word, is_italic)
% convert the character string with categories into actual English characters, e.g. from 'As' (lowercase A) to 'a'
is_first = ~length(word); % check whether this character is the first one of the line of letters
if ((is_italic==1) && (strcmp('Ns', letter)) && (~is_first) && (sum(word(end)=='I'||'J')==1))
    correctletter=[];
elseif (length(letter) == 1)
    % check whether this character could be directly returned, e.g. whether it is 'A', 'B' or '2' rather than 'As', 'Bs' nor even 'ILs1"
    % for speedup didn't set this as the default('otherwise') case of the switch statement
    correctletter = letter; % the most ideal scenario...
else
    switch letter
        case {'As', 'Bs', 'Ds', 'Es', 'Fs', 'Gs', 'Hs', 'Is', 'Js', 'Ms', 'Ns', 'Rs', 'Ts'}
            correctletter = lower(letter(1)); % convert them into lowercase form
        case {'Cc', 'Kk', 'Pp', 'Ss', 'Uu', 'Vv', 'Ww', 'Xx', 'Yy', 'Zz'} % you really can't tell the differences between them
            if (is_first || (length(word)>1 && sum(isstrprop(word,'upper'))>1))
                % if this is the beginning of the line, or if the words beforehand containing more than one capital letters
                % e.g. if letter is 'Yy' and word is 'HE'--'Y' should be added
                correctletter = letter(1);
            else
                %e.g. if letter is 'Ss' and word is 'A'--'s' should be added
                correctletter = letter(2);
            end
        case 'Qs9'
            if (is_first || sum(isstrprop(word,'digit'))>=1)
                correctletter = '9';
            else
                correctletter = 'q';
            end
        case 'Oo0'
            if (~is_first && sum(isstrprop(word(end),'digit'))==1)
                correctletter = letter(3); % if is not at the beginning and the character beforehand is digit, return zero, becasue we usually see 90 not 09
            elseif (is_first || (length(word)>1 && sum(isstrprop(word,'upper'))>1))
                    correctletter = letter(1);
            else
                    correctletter = letter(2);
            end
        case 'ILs1'
            if (~is_first && sum(isstrprop(word(end),'digit'))==1)
                correctletter = letter(4); % if is not at the beginning and the character beforehand is digit, return zero, becasue we usually see 90 not 09
            elseif (is_first || (length(word)>1 && sum(isstrprop(word,'upper'))>1))
                    correctletter = 'I'; % capital i
            else
                    correctletter = 'l'; % lowercase L
            end
        otherwise
            correctletter = '!';
    end
end
end