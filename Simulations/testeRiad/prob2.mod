min:-32*x1+-18* x2+-14* x3+-12* x4+-20* x5+-0* x6+-0* x8+-16* x9+-0* x11+-0* x12+-0* x13+-0* x14+-20* x15+-0* x16+-26* x17+-0* x18+-12* x19+-20* x20+-16* x21;
C1:x1+ x2+x3+x4+x5+x6+x9+x11+x12+x13+x14+x16+x18+x19+x20+x21>=1;
C2:x2+ x1+x3+x4+x5+x6+x12+x17+x21>=1;
C3:x3+ x1+x2+x4+x5+x9+x12>=1;
C4:x4+ x1+x2+x3+x5+x15>=1;
C5:x5+ x1+x2+x3+x4+x9+x16+x18+x20>=1;
C9:x9+ x1+x3+x5+x6+x12+x14+x16>=1;
C15:x15+ x4+x6+x11+x12+x13+x14+x16+x18+x20>=1;
C17:x17+ x1+x6+x11+x12+x13+x14+x15+x16+x18+x19+x20+x21>=1;
C19:x19+ x1+x12+x13+x17+x21>=1;
C20:x20+ x1+x5+x6+x13+x14+x15+x17+x18+x21>=1;
C21:x21+ x1+x2+x13+x14+x17+x18+x20>=1;

C7:x5>=1;

bin x1;
bin x2;
bin x3;
bin x4;
bin x5;
bin x6;
bin x8;
bin x9;
bin x11;
bin x12;
bin x13;
bin x14;
bin x15;
bin x16;
bin x17;
bin x18;
bin x19;
bin x20;
bin x21;
