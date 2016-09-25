%templog1o2

load newgammaWp
mu_fi = exp(gamma(1));
mu_iv = exp(gamma(2));
mu_vn = exp(gamma(3));
mu_vs = mu_vn;
g_i = exp(gamma(4));
g_v = 1;
p0 = exp(gamma(5));%0.001;

%T = 60;
Tsim = 60;
ctrlEveryT = 20;
Tctrl = Tsim*ctrlEveryT;

Am = eye(5) + Tsim*[(-mu_fi-mu_iv)/g_i  mu_iv/g_i                 0          0          1/(60*g_i);
                mu_iv/g_v          (-mu_iv-mu_vn-mu_vs)/g_v  mu_vn/g_v  mu_vs/g_v  0;
                0                         0                   0          0          0;
                0                         0                   0          0          0;
                0                         0                   0          0          0];
Bm = Tsim*[mu_fi/g_i 0 0 0 0]';


Tdata = 14*ones(60*60*24*4/Tsim,2);
%x0 = [data(1,5) data(1,5) data(1,6) data(1,7) p0]';
x0 = [18 18 10 10 p0]';
Tsteps = size(Tdata,1);
t = (1:Tsteps)*Tsim;
Tdata(:,1) = 0+10*cos((t/(60*60)-15 )*2*pi/24);
%Tdata(:,1) = 0+0*cos((t/(60*60)-15 )*2*pi/24);
Tdata(:,2) = Tdata(:,1);

xs = zeros(Tsteps, numel(x0));
us = zeros(Tsteps, 1);
rs = zeros(Tsteps, 1);
xuts = zeros(Tsteps, 1);
x = x0;
r = 22;
%xnp1 = 33000*T;

Aut = eye(2) -Tctrl*[0.0002259 0; 0 0.0002259];
But = Tctrl*eye(2)*0.0001139;
xut = Tdata(1,1:2)'*0.0001139/0.0002259;
staticFors =  0.63-.04;
P = 10;
I = .005;
ImaxT = 5;
eint = 0;
eints = zeros(size(t));
for ii = 1:Tsteps;
    Tdata(ii,1:2) = Tdata(ii,1:2)-20*(t(ii)>70*60*60)*(t(ii)<80*60*60);
    r = 20;
    thod = mod(t(ii)/60/60,24);
    if ((thod >7 && thod < 16.5))
        r = 20;
    end
    if ( thod > 21 || thod < 5)
        r = 20;
    end
    x(3:4) = Tdata(ii,1:2); % utetemp
    
    if (mod(ii, ctrlEveryT) == 1)
        xut = Aut*xut + But*Tdata(ii,1:2)';
        
        eint = eint + I*Tctrl*(r-x(1));
        if (eint > ImaxT)
            eint = ImaxT;
        end
        if (eint < -ImaxT)
            eint = -ImaxT;
        end

        u = r/staticFors;
        %u = u + P*(r-x(1));
        %u = u + eint;
        u = u -sum(xut);

        if (u < x(1))
            u = x(1);
        end
        if (u > 60)
            u = 60;
        end
    end
    
    x = Am*x + Bm*u;
    xs(ii,:) = x';
    us(ii,1) = u;
    rs(ii,1) = r;
    xuts(ii,1) = sum(xut);
    eints(ii) = eint;
end

plot(t/60/60,xs); hold on
legend T_i T_v T_n T_s P
plot(t/60/60,us, 'g--', t/60/60,rs,'b--');
plot(t/60/60,eints,'r--');
plot(t/60/60,xuts,'k--');
hold off;
grid on

