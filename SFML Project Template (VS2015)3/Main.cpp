#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <math.h>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <stdio.h>

using namespace std;
using namespace sf;

const int window_w = 1920;
const int window_h = 1080;
const int l_size = 80;
const int r_size = 30;
const int h_size = 70;
float speed = 50;
const int scale = 1;
RenderWindow window(VideoMode(window_w, window_h), "AVL Tree");
Font font;
View view;
const int def_x = 5;
const int def_y = 20;

int in = 0;
string s;
vector<string> w;

#define Tree Node*

struct Node {
	Node *l, *r;
	int a;
	char s;
	int h;
	Node() { s = '#'; h = 1; a = 0; l = 0; r = 0; }
	int get_h(Tree t) {
		if (!t)return 0;
		return t->h;
	}
	Tree lvl3() {
		if (w[in] == "(") {
			in++;
			Tree res = lvl1();
			if (w[in] != ")") {
				cout << "WRONG" << endl;
				exit(0);
			}
			in++;
			return res;
		}
		if (w[in][0]<'0' || w[in][0]>'9') {
			cout << "WRONG" << endl;
			exit(0);
		}
		Tree res = new Node();
		res->a = stoi(w[in]);
		res->h = 1;
		in++;
		return res;
	}
	Tree lvl2() {
		Tree res = new Node();
		res = lvl3();
		while (w[in] == "*") {
			in++;
			Tree op = new Node();
			op->s = '*';
			op->l = res;
			op->r = lvl3();
			res = op;
			res->h = max(get_h(res->l), get_h(res->r)) + 1;
		}
		return res;
	}
	Tree lvl1() {
		Tree res = new Node();
		res = lvl2();
		while (w[in] == "+" || w[in] == "-") {
			Tree op = new Node();
			op->s = w[in][0];
			in++;
			op->l = res;
			op->r = lvl2();
			res = op;
			res->h = max(get_h(res->l), get_h(res->r)) + 1;
		}
		return res;
	}
	int sol(Tree t) {
		if (!t) return 0;
		if (t->s == '#') return t->a;
		int l_res = sol(t->l);
		int r_res = sol(t->r);
		if (t->s == '+') return l_res + r_res;
		if (t->s == '-') return l_res - r_res;
		if (t->s == '*')return l_res*r_res;
	}
};

void draw(int x, int y, Tree c) {
	CircleShape cir;
	Text text;
	text.setFont(font);
	cir.setOrigin(Vector2f(r_size, r_size));
	cir.setRadius(r_size);
	cir.setOutlineColor(Color::Blue);
	cir.setOutlineThickness(3);
	text.setCharacterSize(144);
	if (c->s == '#') 
		text.setString(to_string(c->a));
	else 
		text.setString(string() + c->s);
	text.setOrigin(Vector2f(text.getLocalBounds().left + text.getLocalBounds().width / 2,
		text.getLocalBounds().top + text.getLocalBounds().height / 2));
	float min_size = min((r_size*1.5f) / text.getLocalBounds().width, (r_size*1.5f / 2) / text.getLocalBounds().height);
	text.setScale(Vector2f(min_size, min_size));
	text.setPosition(Vector2f(x, y));
	text.setColor(Color::Black);
	cir.setPosition(x, y);
	window.draw(cir);
	window.draw(text);
}

void draw_edg(int x1, int y1, int x2, int y2) {
	if (x1 == 0 && y1 == 0)return;
	RectangleShape line(Vector2f(sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)), 6));
	line.setFillColor(Color::Cyan);
	line.setPosition(Vector2f(x1, y1));
	line.rotate(atan2(y2 - y1, x2 - x1) * 180.0 / atan2(0, -1));
	window.draw(line);
}

int get(Tree t, int l, int h) {
	int r = l;
	if (t->l != nullptr)
		r = get(t->l, l, h - 1);
	else
		r += l_size*scale;
	if (t->r != nullptr)
		r = get(t->r, r, h - 1);
	else
		r += l_size*scale;
	if (!t->l && !t->r)
		r += l_size;
	int x_cor = l + (r - l) / 2;
	int y_cor = h * h_size;
	draw(x_cor, y_cor, t);
	return r;
}

pair<int, pair<int, int>> get_edg(Tree t, int l, int h) {
	int r = l;
	pair<int, int> left = make_pair(0, 0);
	pair<int, int> right = make_pair(0, 0);
	if (t->l) {
		pair<int, pair<int, int>> res = get_edg(t->l, l, h - 1);
		r = res.first;
		left = res.second;
	}
	else
		r += l_size*scale;
	if (t->r) {
		pair<int, pair<int, int>> res = get_edg(t->r, r, h - 1);
		r = res.first;
		right = res.second;
	}
	else
		r += l_size*scale;
	if (!t->l && !t->r)
		r += l_size;
	int x_cor = l + (r - l) / 2;
	int y_cor = h * h_size;
	draw_edg(left.first, left.second, x_cor, y_cor);
	draw_edg(right.first, right.second, x_cor, y_cor);
	return make_pair(r, make_pair(x_cor, y_cor));
}

vector<string> parse(string s) {
	vector<string> res;
	for (int i = 0; i < s.size();) {
		if (s[i] == '(') { i++; res.push_back("("); continue; }
		if (s[i] == ')') { i++; res.push_back(")"); continue; }
		if (s[i] == '0') { i++; res.push_back("0"); continue; }
		if (s[i] == ' ') { i++; continue; }
		if (s[i] == '*') { i++; res.push_back("*"); continue; }
		if (s[i] == '+') { i++; res.push_back("+"); continue; }
		if (s[i] == '-') { i++; res.push_back("-"); continue; }
		int n = 0;
		while (s[i] >= '0' && s[i] <= '9') { n = n * 10 + int(s[i++] - '0'); }
		if (n == 0) { cout << "WRONG" << endl; return res; }
		res.push_back(to_string(n));
	}
	res.push_back("#");
	for (int i = 0; i < int(w.size()) - 1; i++) if (w[i][0] >= '0' && w[i][0] <= '9' && w[i + 1][0] >= '0' && w[i + 1][0] <= '9')
	{
		cout << "WRONG" << endl; return res;
	}
	return res;
}

int main() {
	view.reset(FloatRect(0, 0, window_w, window_h));
	srand(time(NULL));
	Clock clock;
	font.loadFromFile("CyrilicOld.TTF");
	bool change = true;
	freopen("input.txt", "r", stdin);
	getline(cin, s);
	w = parse(s);
	Tree t = new Node();
	t = Node().lvl1();
	cout << Node().sol(t) << endl;
	if (w[in] != w.back()) { cout << "WRONG" << endl; return 0; }
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			float del = clock.getElapsedTime().asMicroseconds();
			del = 1;
			clock.restart();
			if (event.type == Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape)) {
				window.close();
			}
			if (Keyboard::isKeyPressed(Keyboard::Z)) {
				view.zoom(1.05f);
				speed = speed*1.05f;
				change = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::X)) {
				view.zoom(0.95f);
				speed = speed*0.95f;
				change = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				view.move(Vector2f(-speed*del, 0));
				change = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::Down)) {
				view.move(Vector2f(0, speed*del));
				change = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::Up)) {
				view.move(Vector2f(0, -speed*del));
				change = true;
			}
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				view.move(Vector2f(speed*del, 0));
				change = true;
			}
			if (change) {
				window.setView(view);
				window.clear(Color(128, 106, 89));
				cout << t->a << ' ' << t->s << endl;
				get_edg(t, 0, t->h);
				get(t, 0, t->h);
				change = false;
				window.display();
			}
		}
	}
	return 0;
}