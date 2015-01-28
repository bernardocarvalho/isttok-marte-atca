function [] = plot_data(board,channel)
numOfChannels = 33;
numOfBoards = 1;

% Load data
load 'data.out';

% Times array
step = numOfBoards*(numOfChannels + 2) + 1;
i = 1:step:length(data);
times = data(i);

% Select right board/channel
start = (board-1)*(numOfChannels + 2) + 2 + channel;
i = start:step:length(data);
channel = data(i);

plot(times,data);


end
