package body pkg_sem is

    procedure wait(sem : in out sem_t) is
    begin
        sem.wait;
    end wait;

    procedure signal(sem : in out sem_t) is 
    begin
        sem.signal;
    end signal;

    protected body sem_t is
        
        entry wait when valor_sem = 1 is
        begin
            valor_sem := 0;
        end wait;

        entry signal when valor_sem = 0 is 
        begin
            valor_sem := 1;
        end signal;

    end sem_t;

end pkg_sem;