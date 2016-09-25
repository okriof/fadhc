function err = kalmantempest3(data, gamma)
% data
mu_fi = gamma(1);
mu_iv = gamma(2);
mu_vn = gamma(3);
g_i = gamma(4);
%g_v = gamma(5);

%p0 = 0;
p0 = gamma(5);

%mu_fi = .000001;
%mu_iv = .00001;
%mu_vn = .000001;

%g_i = .01;
g_v = 1;
mu_vs = mu_vn;

T = 60;

F = eye(5) + T*[(-mu_fi-mu_iv)/g_i  mu_iv/g_i                 0          0          1/(T*g_i);
                mu_iv/g_v          (-mu_iv-mu_vn-mu_vs)/g_v  mu_vn/g_v  mu_vs/g_v  0;
                0                         0                   0          0          0;
                0                         0                   0          0          0;
                0                         0                   0          0          0];
             
B = T*[mu_fi/g_i 0 0 0 0]';

x0 = [data(1,5) data(1,5) data(1,6) data(1,7) p0]';

Tsteps = size(data,1);
t = (1:Tsteps)*T;
xs = zeros(Tsteps, numel(x0));
x = x0;
for ii = 1:Tsteps;
    x(3:4) = data(ii,6:7);
    x = F*x + B*data(ii,3);
    xs(ii,:) = x';
end

plot(t,xs); hold on
legend T_i T_v T_n T_s P
plot(t,data(:,3), 'm--', t,data(:,4), 'm-.');
plot(t,data(:,5), 'b--');
hold off;
pause(.1);

err = xs(100:end, 1) - data(100:end,5);

