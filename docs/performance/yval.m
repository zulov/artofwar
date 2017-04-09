function retval = yval(path)
  PERCENT = 10;
  p1  = load(strcat(path,"perf01.txt"));
  p2  = load(strcat(path,"perf02.txt"));
  p3  = load(strcat(path,"perf03.txt"));
  p4  = load(strcat(path,"perf04.txt"));
  p5  = load(strcat(path,"perf05.txt"));
  p6  = load(strcat(path,"perf06.txt"));
  p7  = load(strcat(path,"perf07.txt"));
  p8  = load(strcat(path,"perf08.txt"));
  p9  = load(strcat(path,"perf09.txt"));
  p10 = load(strcat(path,"perf10.txt"));
  p11 = load(strcat(path,"perf11.txt"));

  mp1  = mean(prctile(sort(p1 (:,1)),PERCENT));
  mp2  = mean(prctile(sort(p2 (:,1)),PERCENT));
  mp3  = mean(prctile(sort(p3 (:,1)),PERCENT));
  mp4  = mean(prctile(sort(p4 (:,1)),PERCENT));
  mp5  = mean(prctile(sort(p5 (:,1)),PERCENT));
  mp6  = mean(prctile(sort(p6 (:,1)),PERCENT));
  mp7  = mean(prctile(sort(p7 (:,1)),PERCENT));
  mp8  = mean(prctile(sort(p8 (:,1)),PERCENT));
  mp9  = mean(prctile(sort(p9 (:,1)),PERCENT));
  mp10 = mean(prctile(sort(p10(:,1)),PERCENT));
  mp11 = mean(prctile(sort(p11(:,1)),PERCENT));
    retval = [1/mp1  1/mp2  1/mp3  1/mp4  1/mp5  1/mp6  1/mp7  1/mp8  1/mp9  1/mp10 1/mp11];

endfunction