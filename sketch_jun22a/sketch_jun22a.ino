

extern "C"
{
    struct _t
    {
        struct _a
        {
            struct _b
            {

                int c;
            } ml2;

            int b;
        } ml1[2];

        int a;
    };

    struct _t ml0[1]=
    {
        [0]={

            ml1[0]={
                .ml2={.c=1},
                .b=2
            },
            ml1[1]={
                .ml2={.c=3},
                .b=4
            },

            .a = 5
        },

        

    };
}

void setup() {

  Serial.begin(230400);

    
}

void loop() 
{


}
