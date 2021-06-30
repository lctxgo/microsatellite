function [dvarO,JO,q] = Linear_CVP
clear;clc;

X_q0 = 0.866;
X_q1 = 0;
X_q2 = 0.5;
X_q3 = 0;
X_wx = 0;
X_wy = 0;
X_wz = 0;
Tx = 0;
Ty = 0;
Tz = 0; 
h0 = 0.25;
beta = 53.14;
a = cosd(beta);
b = sind(beta);
Ix = 1.5;
Iy = 1.5;
Iz = 1.5;

T = 20; % terminal time
N = 5; % number of control stages
rho = 100; % weight on missing the final target

% x0 = zeros(12,1); % initial state
% x0 = [zeros(4,1);1;zeros(7,1)];
x0 = [zeros(4,1);1;zeros(7,1)];
ts = 0:(T/N):T;

% Options for ODE & NLP Solvers
% %optODE = odeset( 'RelTol', 1e-4, 'AbsTol', 1e-4 );
% %optNLP = optimset( 'LargeScale', 'off', 'GradObj','off', 'GradConstr','off',...
%     'DerivativeCheck', 'off', 'Display', 'iter', 'TolX', 1e-4,...
%     'TolFun', 1e-4, 'TolCon', 1e-4, 'MaxFunEvals',5000,...
%     'DiffMinChange',1e-4,'Algorithm','sqp');%interior-point

optODE = odeset( 'RelTol', 1e-4, 'AbsTol', 1e-4 );
optNLP = optimset( 'LargeScale', 'off', 'GradObj','off', 'GradConstr','off',...
    'DerivativeCheck', 'off', 'Display', 'iter', 'TolX', 1e-4,...
    'TolFun', 1e-4, 'TolCon', 1e-3, 'MaxFunEvals',5000,...
    'DiffMinChange',1e-4,'Algorithm','sqp');%interior-point

%% Continuous linear spline control
% In this section, control $u$ and $\theta$ are assumed to be continuous 
% linear spline in each stage of uniform length.
dvar0 = [repmat(0,1,4*(N+1)),X_q0,X_q1,X_q2,X_q3,X_wx,X_wy,X_wz]; 
% design variables contains $N+1$ 
% start and end points of control signal $u$, $\theta$ and the final 
% position
lb = -Inf(1,4*(N+1)+7);
lb(1:4*(N+1)) = -1.3; % enforce lower bound on control signal $u$
ub = Inf(1,4*(N+1)+7);
ub(1:4*(N+1)) = 1.3; % enforce upper bound on control signal $u$

% Sequential Approach of Dynamic Optimization
tic
[dvarO,JO] = fmincon(@(dvar)  costfun(X_q0,X_q1,X_q2,X_q3,Tx, Ty, Tz, h0,a,b,Ix, Iy, Iz, x0, ts, dvar, rho, N,  optODE ),...
    dvar0,[],[],[],[],lb,ub,...
    @(dvar) confun(Tx, Ty, Tz,  h0,a,b,Ix, Iy, Iz, x0, dvar, ts, N, optODE),optNLP);
toc
disp(['运行时间: ',num2str(toc)]);
q = dvarO(4*(N+1)+1:end);
 [topt,xopt,u1opt,u2opt,u3opt,u4opt]=plotopt(Tx, Ty, Tz, h0,a,b,Ix, Iy, Iz, x0,N,ts,dvarO,rho,optODE );
end

function dxdt = dyneqn(Tx, Ty, Tz, h0,a,b,Ix, Iy, Iz, t,x,u1,u2,u3,u4,ts,ks )
%DYNEQN2 dynmaic equation of the system with continuous linear spline control
dxdt = [zeros(4,1);1;zeros(7,1)];

u1i = interp1([ts(ks) ts(ks+1)],[u1(ks) u1(ks+1)],t);
u2i = interp1([ts(ks) ts(ks+1)],[u2(ks) u2(ks+1)],t);
u3i = interp1([ts(ks) ts(ks+1)],[u3(ks) u3(ks+1)],t);
u4i = interp1([ts(ks) ts(ks+1)],[u4(ks) u4(ks+1)],t);

U = [u1i,u2i,u3i,u4i];

dxdt(1) = u1i;
dxdt(2) = u2i;
dxdt(3) = u3i;
dxdt(4) = u4i;

dxdt(5) = 0.5*(-x(6)*x(9)-x(7)*x(10)-x(8)*x(11));   %q0
dxdt(6) = 0.5*(x(5)*x(9)-x(8)*x(10)+x(7)*x(11));    %q1
dxdt(7) = 0.5*(x(8)*x(9)+x(5)*x(10)-x(6)*x(11));    %q2
dxdt(8) = 0.5*(-x(7)*x(9)+x(6)*x(10)+x(5)*x(11));   %q3

hx = h0*(-a*sin(x(1)) - cos(x(2)) + a*sin(x(3)) + cos(x(4)));
hy = h0*(cos(x(1)) - a*sin(x(2)) - cos(x(3)) + a*sin(x(4)));
hz = h0*b*(sin(x(1)) + sin(x(2)) + sin(x(3)) + sin(x(4)));
dhx = h0*(-a*cos(x(1))*u1i + sin(x(2))*u2i +a*cos(x(3))*u3i - sin(x(4))*u4i);
dhy = h0*(-sin(x(1))*u1i - a*cos(x(2))*u2i + sin(x(3))*u3i + a*cos(x(4))*u4i);
dhz = h0*b*(cos(x(1))*u1i + cos(x(2))*u2i + cos(x(3))*u3i + cos(x(4))*u4i);

dxdt(9) = ((Iy-Iz)*x(10)*x(11) - (x(10)*hz-x(11)*hy) - dhx + Tx)/Ix;
dxdt(10) = ((Iz-Ix)*x(9)*x(11) - (x(11)*hx-x(9)*hz) - dhy + Ty)/Iy;
dxdt(11) = ((Ix-Iy)*x(9)*x(10) - (x(9)*hy-x(10)*hx) - dhz + Tz)/Iz;

dxdt(12) = 0.5*(U*U');%指标函数中的被积函数

end

function f = myfunint(Tx, Ty, Tz,  h0,a,b,Ix, Iy, Iz, x0,N,ts,dvar,optODE )
%MYFUNINT2 integration function for continuous linear spline control
z0 = x0;
% u = dvar(1:N+1);
% theta = dvar(N+2:2*(N+1));
u1 = dvar(1:N+1);
u2 = dvar(N+1+1:2*(N+1));
u3 = dvar(2*(N+1)+1:3*(N+1));
u4 = dvar(3*(N+1)+1:4*(N+1));

for ks = 1:N
    [tspan,zs] = ode15s( @(t,x)dyneqn(Tx, Ty, Tz, h0,a,b,Ix, Iy, Iz, t,x,u1,u2,u3,u4,ts,ks ), ...
        [ts(ks),ts(ks+1)], z0, optODE );
    z0 = zs(end,:)';
end
f = zs(end,:)';

end

function J = costfun(X_q0,X_q1,X_q2,X_q3,Tx, Ty, Tz, h0,a,b,Ix, Iy, Iz, x0, ts, dvar, rho, N,  optODE )
%COSTFUN2 cost function for continuous linear spline control
f = myfunint( Tx, Ty, Tz, h0,a,b,Ix, Iy, Iz, x0,N,ts,dvar,optODE );
q0T = dvar(4*(N+1)+1);
q1T = dvar(4*(N+1)+2);
q2T = dvar(4*(N+1)+3);
q3T = dvar(4*(N+1)+4);
wxT = dvar(4*(N+1)+5);
wyT = dvar(4*(N+1)+6);
wzT = dvar(4*(N+1)+7);


J = rho*((q0T-X_q0)^2+(q1T-X_q1)^2+(q2T-X_q2)^2+(q3T-X_q3)^2+...
    (wxT)^2+(wyT)^2+(wzT)^2) + f(end);%
end

function [c, ceq] = confun(Tx, Ty, Tz,  h0,a,b,Ix, Iy, Iz, x0, dvar, ts, N, optODE)
%CONFUN2 constraint function for continuous linear spline control
f = myfunint(Tx, Ty, Tz,  h0,a,b,Ix, Iy, Iz, x0,N,ts,dvar,optODE );

ceq(1) = dvar(4*(N+1)+1) - f(5);
ceq(2) = dvar(4*(N+1)+2) - f(6); 
ceq(3) = dvar(4*(N+1)+3) - f(7);
ceq(4) = dvar(4*(N+1)+4) - f(8);
ceq(5) = dvar(4*(N+1)+5) - f(9);
ceq(6) = dvar(4*(N+1)+6) - f(10);
ceq(7) = dvar(4*(N+1)+7) - f(11);% for final position, the value of design 
% varialbes must be the same as the ones come out from integration
C = [-a*cos(f(1)),sin(f(2)),a*cos(f(3)),-sin(f(4));...
     -sin(f(1)),-a*cos(f(2)),sin(f(3)),a*cos(f(4));...
     b*cos(f(1)),b*cos(f(2)),b*cos(f(3)),b*cos(f(4))];
 
c = 0.5-det(C*C');
end

function [topt,xopt,u1opt,u2opt,u3opt,u4opt]=plotopt(Tx, Ty, Tz, h0,a,b,Ix, Iy, Iz, x0,N,ts,dvarO,rho,optODE )
%PLOTOPT2 plot function for continuous linear spline control
z0 =  x0;
topt = [];
xopt = [];
u1opt = [];
u2opt = [];
u3opt = [];
u4opt = [];
Dopt = [];
u1 = dvarO(1:N+1);
u2 = dvarO(N+1+1:2*(N+1));
u3 = dvarO(2*(N+1)+1:3*(N+1));
u4 = dvarO(3*(N+1)+1:4*(N+1));

% forward state integration
for ks = 1:N
    [tspan,zs] = ode15s( @(t,x)dyneqn(Tx, Ty, Tz, h0,a,b,Ix, Iy, Iz, t,x,u1,u2,u3,u4,ts,ks ), ...
        [ts(ks),ts(ks+1)], z0, optODE );
    z0 = zs(end,:)';
    [m,n] = size(zs);
    D = [];
%     disp(zs);
%     disp('\n');
    for i = 1:m  
        iter_x = zs(i,:);
        C = [-a*cos(iter_x(1)),sin(iter_x(2)),a*cos(iter_x(3)),-sin(iter_x(4));...
         -sin(iter_x(1)),-a*cos(iter_x(2)),sin(iter_x(3)),a*cos(iter_x(4));...
         b*cos(iter_x(1)),b*cos(iter_x(2)),b*cos(iter_x(3)),b*cos(iter_x(4))];
        D = [D;det(C*C')];
    end
    Dopt = [ Dopt; D ]
    topt = [ topt; tspan ];
    xopt = [ xopt; zs ];
    u1opt = [ u1opt; interp1([ts(ks) ts(ks+1)],[u1(ks) u1(ks+1)],tspan); ];
%     thetaopt = [thetaopt; interp1([ts(ks) ts(ks+1)],[theta(ks) theta(ks+1)],tspan) ];
    u2opt = [ u2opt; interp1([ts(ks) ts(ks+1)],[u2(ks) u2(ks+1)],tspan); ];
    u3opt = [ u3opt; interp1([ts(ks) ts(ks+1)],[u3(ks) u3(ks+1)],tspan); ];
    u4opt = [ u4opt; interp1([ts(ks) ts(ks+1)],[u4(ks) u1(ks+1)],tspan); ];
    Mxopt = -a*cos(xopt(:,1)).*u1opt + sin(xopt(:,2)).*u2opt +...
            a*cos(xopt(:,3)).*u3opt - sin(xopt(:,4)).*u4opt;
    Myopt = -sin(xopt(:,1)).*u1opt - a*cos(xopt(:,2)).*u2opt +...
            sin(xopt(:,3)).*u3opt + a*cos(xopt(:,4)).*u4opt;
    Mzopt = b*(cos(xopt(:,1)).*u1opt + cos(xopt(:,2)).*u2opt +...
            cos(xopt(:,3)).*u3opt + cos(xopt(:,4)).*u4opt);
end

% Display Optimal Cost
disp(sprintf('Optimal cost for N=%d: %d', N, zs(end,12)));
%% state trajectory
figure(1) % state trajectory

% plot(topt,xopt(:,5),'k-','LineWidth',1,'Marker','o')
plot(topt,xopt(:,5),'LineWidth',2)
hold on
xlabel('t/s','fontsize',14)
ylabel('q','fontsize',14)
% set(gca,'FontSize',12)

% plot(topt,xopt(:,6),'k-','LineWidth',1,'Marker','+')
plot(topt,xopt(:,6),'LineWidth',2)
hold on

% plot(topt,xopt(:,7),'k-','LineWidth',1,'Marker','*')
plot(topt,xopt(:,7),'LineWidth',2)
hold on

% plot(topt,xopt(:,8),'k-','LineWidth',1,'Marker','x')
plot(topt,xopt(:,8),'LineWidth',2)
legend('q_{1}','q_{2}','q_{3}','q_{4}')
%% gimbal u plot
figure(2) % control signal $u$
subplot(4,1,1)
u1opt = u1opt*1000;
plot(topt,u1opt,'k-','LineWidth',2)
xlabel('t/s','fontsize',14)
ylabel('u_{1}/(rad/s)','fontsize',14)
set(gca,'FontSize',12)

%saveas(gcf,['u(t)_',int2str(ns),'stg.eps'], 'psc2')

subplot(4,1,2)
plot(topt,u2opt,'k-','LineWidth',2)
% xlabel('\it t/\rm s','fontsize',14)
% ylabel('\it u_{2}(t)\rm (rad/s)','fontsize',14)
xlabel('t/s','fontsize',14)
ylabel('u_{2}/(rad/s)','fontsize',14)
set(gca,'FontSize',12)


subplot(4,1,3)
u3opt = u3opt*10000;
plot(topt,u3opt,'k-','LineWidth',2)
xlabel('t/s','fontsize',14)
ylabel('u_{3}/(rad/s)','fontsize',14)
set(gca,'FontSize',12)


subplot(4,1,4)
plot(topt,u4opt,'k-','LineWidth',2)
xlabel('t/s','fontsize',14)
ylabel('u_{4}/(rad/s)','fontsize',14)
ylim([-0.15 0.15])
set(gca,'FontSize',12)
%% D plot

figure(3) % state trajectory
plot(topt,Dopt,'k-','LineWidth',1)
xlabel('t/s','fontsize',14) 
ylabel('D','fontsize',14)
set(gca,'FontSize',12)

%% w plot
figure(4) % w

%plot(topt,(180/pi)*xopt(:,9),'k-','LineWidth',1,'Marker','o')
% plot(topt,xopt(:,9),'k-','LineWidth',1,'Marker','o')
plot(topt,(180/pi)*xopt(:,9),'LineWidth',2)
hold on
xlabel('t/s','fontsize',14)
%ylabel('\omega(t)(\circ/\rm s)','fontsize',14)
ylabel('\omega/(\circ/s)','fontsize',14)

%plot(topt,(180/pi)*xopt(:,10),'k-','LineWidth',1,'Marker','*')
% plot(topt,xopt(:,10),'k-','LineWidth',1,'Marker','*')
plot(topt,(180/pi)*xopt(:,10),'LineWidth',2)
hold on


%plot(topt,(180/pi)*xopt(:,11),'k-','LineWidth',1,'Marker','x')
% plot(topt,xopt(:,11),'k-','LineWidth',1,'Marker','x')
plot(topt,(180/pi)*xopt(:,11),'LineWidth',2)
legend('\omega_{x}','\omega_{y}','\omega_{z}')

%% M torque plot
% figure(5) % control signal $\theta$
% plot(topt,Mxopt,'k-','LineWidth',1,'Marker','o')
% hold on
% xlabel('\it t/\rm s','fontsize',14)
% ylabel('\rm M(t)(\rm Nm)','fontsize',14)
% plot(topt,Myopt,'k-','LineWidth',1,'Marker','*')
% hold on
% plot(topt,Mzopt,'k-','LineWidth',1,'Marker','x')
% legend('M_{x}','M_{y}','M_{z}')

%% \delta plot
figure(6) % state trajectory

% plot(topt,(180/pi)*xopt(:,1),'k-','LineWidth',1,'Marker','o')
plot(topt,(180/pi)*xopt(:,1),'LineWidth',2)
hold on
xlabel('t/s','fontsize',14)
ylabel('\delta/\circ','fontsize',14)
% set(gca,'FontSize',12)

% plot(topt,(180/pi)*xopt(:,2),'k-','LineWidth',1,'Marker','+')
plot(topt,(180/pi)*xopt(:,2),'LineWidth',2)
hold on

% plot(topt,(180/pi)*xopt(:,3),'k-','LineWidth',1,'Marker','*')
plot(topt,(180/pi)*xopt(:,3),'LineWidth',2)
hold on

% plot(topt,(180/pi)*xopt(:,4),'k-','LineWidth',1,'Marker','x')
plot(topt,(180/pi)*xopt(:,4),'LineWidth',2)
legend('\delta_{1}','\delta_{2}','\delta_{3}','\delta_{4}')

end
