struct
{
    struct
    {
        struct
        {
            int c;
        } s2[2];

        int c;
    } s1[2];

    int b;
}
s0[2]=
{

    s0[0]=
    {
        {
            s0[0].s1[0]=
                {
                    s0[0].s1[0].s2[0]={c:1},
                    s0[0].s1[0].s2[1]={c:2},
                c:3
                },

            s0[0].s1[1]=
                {
                    s0[0].s1[1].s2[0]={c:4},
                    s0[0].s1[1].s2[1]={c:5},
                c:6
                },
        },

        b:7
    },

    s0[1]=
    {
        {
            s0[1].s1[0]=
                {
                    s0[1].s1[0].s2[0]={c:10},
                    s0[1].s1[0].s2[1]={c:11},
                c:12
                },

            s0[1].s1[1]=
                {
                    s0[1].s1[1].s2[0]={c:13},
                    s0[1].s1[1].s2[1]={c:14},
                c:15
                },
        },

        b:16
    },


};



void setup() {
  // put your setup code here, to run once:
 Serial.begin(230400);
 
    Serial.println(s0[0].s1[0].s2[0].c);
    Serial.println(s0[0].s1[0].s2[1].c);
    Serial.println(s0[0].s1[1].s2[0].c);
    Serial.println(s0[0].s1[1].s2[1].c);
    //
    
    Serial.println(s0[1].s1[0].s2[0].c);
    Serial.println(s0[1].s1[0].s2[1].c);
    Serial.println(s0[1].s1[1].s2[0].c);
    Serial.println(s0[1].s1[1].s2[1].c);
}

void loop() {
  // put your main code here, to run repeatedly:

}
