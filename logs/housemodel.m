load newgammaWp
mu_fi = exp(gamma(1));
mu_iv = exp(gamma(2));
mu_vn = exp(gamma(3));
mu_vs = mu_vn;
g_i = exp(gamma(4));
g_v = 1;

s = tf('s');

A = [(-mu_fi-mu_iv)/g_i mu_iv/g_i;
      mu_iv/g_v  (-mu_iv-mu_vn-mu_vs)];
  
B = [mu_fi/g_i; 0];

F = [0 0; mu_vn/g_v mu_vs/g_v];

C = [1 0];

G = C*inv(eye(2)*s-A)*B;

Gf = C*inv(eye(2)*s-A)*F;

Gf1 = C*inv(eye(2)*s-A)*F(:,1);

Gfwd = minreal(Gf1/G);


%% integration
Ap = [A [0;0]; -C 0];
Bp = [B;0];
Rp = [0; 0; 1];
Cp = [0 0 1];
%pzmap(C*inv(eye(2)*s-A)*B);
%pzmap(Cp*inv(eye(3)*s-Ap)*Bp);

%L = place(Ap,Bp,-[1 1.1 1.2]*4*10^-4)
L = place(Ap,Bp,-[4 1 .1]*10^-4)
pzmap(Cp*inv(eye(3)*s -Ap + Bp*L)*Rp);