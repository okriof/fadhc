%templog1o2

load newgammaWp
mu_fi = exp(gamma(1));
mu_iv = exp(gamma(2));
mu_vn = exp(gamma(3));
mu_vs = mu_vn;
g_i = exp(gamma(4));
g_v = 1;
p0 = exp(gamma(5));%0.001;

T = 60;

Am = eye(5) + T*[(-mu_fi-mu_iv)/g_i  mu_iv/g_i                 0          0          1/(T*g_i);
                mu_iv/g_v          (-mu_iv-mu_vn-mu_vs)/g_v  mu_vn/g_v  mu_vs/g_v  0;
                0                         0                   0          0          0;
                0                         0                   0          0          0;
                0                         0                   0          0          0];
Bm = T*[mu_fi/g_i 0 0 0 0]';


Tdata = 14*ones(60*60*24*4/T,2);
x0 = [data(1,5) data(1,5) data(1,6) data(1,7) p0]';
Tsteps = size(Tdata,1);
t = (1:Tsteps)*T;
Tdata(:,1) = 5+5*cos((t/(60*60)-15 )*2*pi/24);
Tdata(:,2) = Tdata(:,1);

xs = zeros(Tsteps, numel(x0));
us = zeros(Tsteps, 1);
rs = zeros(Tsteps, 1);
x = x0;
r = 22;
xnp1 = 33000*T;

%L =[4.427771464324012   3.699317848359919  -0.002116825246325];
%L =[-1.674993337351447   7.440778493367121  -0.000605412020449];
%L =[5.953462664742649  -8.773896010486924  -0.003725612433531];
%L = [2.139234663695693  10.996364665319899  -0.000141121683088];
Aut = eye(2) -T*[7.207e-05 0; 0 7.207e-05];
But = T*eye(2)*2.088e-05;
xut = data(1,6:7)'*2.088e-05/7.207e-05;
staticFors =  0.6330;
P = 25;
I = .005;
ImaxT = 5;
eint = 0;
eints = zeros(size(t));
for ii = 1:Tsteps;
    Tdata(ii,1:2) = Tdata(ii,1:2)-20*(t(ii)>70*60*60)*(t(ii)<80*60*60);
    r = 20;
    thod = mod(t(ii)/60/60,24);
    %if ((t(ii)>60*60*30)*(t(ii)<60*60*38))
    if ((thod >7 && thod < 16.5))
        r = 18;
    end
    if ( thod > 21 || thod < 5)
        r = 20;
    end
    x(3:4) = Tdata(ii,1:2); % utetemp
    
    xut = Aut*xut + But*Tdata(ii,1:2)';
    xctrl = [x(1:2); xnp1];
    
    %u = -L*xctrl -sum(xut); % tillståndsåterkoppling
    
    eint = eint + T*(r-x(1));
    if (eint*I > ImaxT)
        eint = ImaxT/I;
    end
    if (eint*I < -ImaxT)
        eint = -ImaxT/I;
    end
    
    u = r/staticFors+ P*(r-x(1)) + I*eint -sum(xut); %pid
    if (u < x(1))
        u = x(1);
        %eint = (u-P*(r-x(1)) + sum(xut))/I;
        %xnp1 = (-u-sum(xut)-L(1:2)*xctrl(1:2))/L(3);
        %u = -L*[x(1:2); xnp1] -sum(xut)
    end
    %if (u > 100)
    %    u = 100;
    %    xnp1 = (-u-sum(xut)-L(1:2)*xctrl(1:2))/L(3);
    %    %u = -L*[x(1:2); xnp1] -sum(xut)
    %end
 
    %if (u < x(1))
    %    u = x(1);
    %end
    if (u > 60)
        u = 60;
    end
    
    x = Am*x + Bm*u;
    xnp1 = xnp1 + T*(r-x(1));
    xs(ii,:) = x';
    us(ii,1) = u;
    rs(ii,1) = r;
    eints(ii) = eint;
end

plot(t/60/60,xs); hold on
legend T_i T_v T_n T_s P
plot(t/60/60,us, 'g--', t/60/60,rs,'b--');
plot(t/60/60,I*eints,'r--');
hold off;
grid on

