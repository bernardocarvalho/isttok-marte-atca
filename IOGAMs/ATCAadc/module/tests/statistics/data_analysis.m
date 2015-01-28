clc;
clear all;
close all;

%% Configuration
numberOfBoards = 2;
numberOfDMAs   = 4;
masterBoard    = 1;

%% Extract data
load 'data.out';

i = 1:(2*numberOfBoards*numberOfDMAs+1):length(data);
times = data(i);

values = zeros(length(times),2*numberOfBoards*numberOfDMAs);
counter = 0;

for t=1:(2*numberOfBoards*numberOfDMAs+1):length(data)
    counter=counter+1;
    for i=1:numberOfBoards
        for j=1:numberOfDMAs
            position = (i-1)*(2*numberOfDMAs);
            values(counter,(position+j*2-1)) = data(t+position+j*2-1);
            values(counter,(position+j*2)) = data(t+position+j*2);
        end
    end
end

%% Check frequency
output_mean = zeros(numberOfBoards*numberOfDMAs*2,1);
for i=1:numberOfBoards*numberOfDMAs*2
    % Find indexes where header/footer changes
    tmp = values(1:end-1,i) - values(2:end,i);
    % Obtain indices of header/footer change
    indexes = find(tmp==-4) + 1;
    % Check that there are no jumps
    %indexes_jump = find(tmp~=0);
    %if(mean(indexes_jump == indexes) ~= 1) 
    %    sprintf('THERE ARE SOME JUMPS!\n')
    %end
    indexes_shift = indexes(2:end);
    indexes = indexes(1:end-1);
    % Find time between a cheange and another
    time_difference = times(indexes_shift) - times(indexes);
    % Calculate mean
    time_difference = time_difference / numberOfDMAs;
    time_mean = sum(time_difference) / length(indexes);
    sprintf('Mean buffer change time (%d) = %f',i,time_mean)
    output_mean(i) = time_mean;
end
figure(1);
plot(output_mean);
title('Time between an index change (mean) for each DMA buffer');


%% Check for errors
% Build value matrix for master board
master_index = 1+ ((masterBoard-1) * numberOfDMAs * 2);
master_matrix = values(:,master_index:(numberOfDMAs*2)+master_index-1);
% Find indexes of header change
for i=1:2:(numberOfDMAs*2)
    tmp = master_matrix(1:end-1,i) - master_matrix(2:end,i);
    indexes = find(tmp~=0) + 1;
    
    for j=1:length(indexes)
        t = times(indexes(j));
        t_index = find(times==t);
        header_master = 0;
        dma_index = 0;
        % find maximum header number
        for k=1:(2*numberOfDMAs)
            if (master_matrix(t_index,k) > header_master)
                header_master = master_matrix(t_index,k);
                dma_index = k;
            end
        end
        %check for header error
        for k=1:numberOfBoards
            index = (k-1) * numberOfDMAs * 2;
            if (values(t_index,index+dma_index) ~= header_master)
                sprintf('WRONG HEADER ON ROW %d\n',t_index)
            end
        end
    end
end
