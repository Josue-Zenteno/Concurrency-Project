with pkg_sem; use pkg_sem;
with Text_IO; use Text_IO;
with Ada.Integer_Text_IO; use Ada.Integer_Text_IO;
with Ada.Numerics.Discrete_Random;

procedure main is

    --Generating Random Durations
    Random_Duration : Duration;
    type Custom is range 100..700;
    package Rand_Cust is new Ada.Numerics.Discrete_Random(Custom);
    use Rand_Cust;
    Seed:Generator;
    Num:Custom;

    --Dealing with the exercise
    type semaphore is access sem_t;

    pot : Integer := 8;
    
    N_cooks : constant Integer := 1;
    N_canibals : Integer;
    
    mutex : sem_t;
    pot_empty : sem_t;
    pot_full : sem_t;

    task type canibal (Id: Character);
    task body canibal is

        procedure Eat is
        begin

            wait(mutex);

            if pot <= 0 Then

                signal(pot_empty);
                wait(pot_full);

            else

                Put(Id & "eat  ");
                pot := pot - 1;
                delay 2.0;
            end if;

            signal(mutex);
            

        end Eat;

        procedure Dance is
        begin

            Reset(Seed);
            Num := Random(Seed);
            Random_Duration := Duration(Num)/100;

            Put(Id & "dance ");
            delay Random_Duration;

        end Dance;

        begin

            loop
                Eat;Dance;
            end loop;

    end canibal;


    task type cook (Id: Character);

    task body cook is

        procedure cock is
        begin
            wait(pot_empty);

            Put(Id & "cocking ");delay 1.0;
            pot := 8;

            signal(pot_full);

        end cock;

        begin

            loop
                cock;
            end loop;

    end cook;


    begin
        
        Put_Line("Number of cooks: 1");
        Put("Number of canibals: ");Get(N_canibals);New_Line;
        
        declare

            type PCanibal is access canibal;
            type PCook is access cook;
            P: PCook;
            C: Character := 'A';
            Tribe : array (1..N_canibals) of PCanibal;

        begin
            signal(mutex);
            P := new cook ('Z');

            for i in 1..N_canibals loop

                Tribe(i) := new canibal(C);
                C := Character'Succ(C);

            end loop;

        end;

end main;