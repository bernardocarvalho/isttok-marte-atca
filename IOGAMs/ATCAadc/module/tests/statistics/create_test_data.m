fid = fopen('data_test.out','wt');

for i=4:100
    for j=1:4
        p1 = i;
        p2 = i;
        p3 = i;
        p4 = i;
        if(mod(i,4)==0)
            p1=p1-3;
            p2=p2-2;
            p3=p3-1;
            p4=p4;
        end
        if(mod(i,4)==1)
            p1=p1;
            p2=p2-3;
            p3=p3-2;
            p4=p4-1;
        end
        if(mod(i,4)==2)
            p1=p1-1;
            p2=p2;
            p3=p3-3;
            p4=p4-2;
        end
        if(mod(i,4)==3)
            p1=p1-2;
            p2=p2-1;
            p3=p3;
            p4=p4-3;
        end
    end

    fprintf(fid,'%d\n',i*50);
    fprintf(fid,'%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n',p1,p1,p2,p2,p3,p3,p4,p4);
    fprintf(fid,'%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n',p1,p1,p2,p2,p3,p3,p4,p4);
end

fclose(fid);