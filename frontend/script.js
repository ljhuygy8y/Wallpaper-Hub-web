class WallpaperHub {
  constructor() {
    this.apiUrl = 'http://127.0.0.1:8080/api';
    this.user = null;
    this.locale = 'uk';
    this.currentCategory = 'all';
    this.wallpaperCache = {};
    this.friendProfile = null;
    this.categoryOrder = ['авто', 'природа', 'тварини', 'море', 'космос', 'абстракція'];
    this.categoryLabels = {
      uk: { авто: 'Машини', природа: 'Природа', тварини: 'Тварини', море: 'Море', космос: 'Космос', абстракція: 'Абстракція' },
      en: { авто: 'Cars', природа: 'Nature', тварини: 'Animals', море: 'Sea', космос: 'Space', абстракція: 'Abstract' }
    };
    this.sampleWallpapers = this.createSamples();
    this.translations = {
      uk: {
        appName: 'Wallpaper Hub', welcome: 'Ласкаво просимо', loginTitle: 'Вхід у систему',
        registerTitle: 'Новий акаунт', username: "Ім'я користувача", password: 'Пароль',
        signIn: 'Увійти', signUp: 'Зареєструватися', profileName: "Ім'я профілю",
        language: 'Мова', theme: 'Тема', themeDark: 'Темна', themeLight: 'Світла',
        createAccount: 'Створити акаунт', backToLogin: 'Назад до входу', navLibrary: 'Обої',
        navFriends: 'Друзі', navProfile: 'Профіль', navSettings: 'Налаштування', logout: 'Вихід',
        friends: 'Друзі', searchUsers: 'Пошук користувачів', searchPlaceholder: "Введіть ім'я...",
        search: 'Шукати', editProfile: 'Редагувати профіль', saveProfile: 'Зберегти профіль',
        changePassword: 'Змінити пароль', currentPassword: 'Поточний пароль', newPassword: 'Новий пароль',
        toggleTheme: 'Світла / темна тема',
        tabLibrary: 'Бібліотека', tabCollection: 'Моя колекція', uploadButton: 'Завантажити свої обої',
        uploadModalTitle: 'Завантажити шпалери', chooseCategory: '-- Оберіть категорію --',
        uploadName: 'Назва', uploadCategory: 'Категорія',
        uploadCancel: 'Скасувати', uploadSubmit: 'Завантажити', catAll: 'Всі'
      },
      en: {
        appName: 'Wallpaper Hub', welcome: 'Welcome back', loginTitle: 'Sign In',
        registerTitle: 'New Account', username: 'Username', password: 'Password',
        signIn: 'Sign In', signUp: 'Sign Up', profileName: 'Profile Name',
        language: 'Language', theme: 'Theme', themeDark: 'Dark', themeLight: 'Light',
        createAccount: 'Create Account', backToLogin: 'Back to Login', navLibrary: 'Wallpapers',
        navFriends: 'Friends', navProfile: 'Profile', navSettings: 'Settings', logout: 'Logout',
        friends: 'Friends', searchUsers: 'Search Users', searchPlaceholder: 'Enter name...',
        search: 'Search', editProfile: 'Edit Profile', saveProfile: 'Save Profile',
        changePassword: 'Change Password', currentPassword: 'Current Password', newPassword: 'New Password',
        toggleTheme: 'Light / Dark Theme',
        tabLibrary: 'Library', tabCollection: 'My collection', uploadButton: 'Upload your wallpapers',
        uploadModalTitle: 'Upload wallpapers', chooseCategory: '-- Choose category --',
        uploadName: 'Name', uploadCategory: 'Category',
        uploadCancel: 'Cancel', uploadSubmit: 'Upload', catAll: 'All'
      }
    };
    this.init();
  }

  createSamples() {
    const result = {};
    const photos = {
      авто: {
        prefix: 'auto',
        ids: ['1492144534655-ae79c964c9d7', '1503376780353-7e6692767b70', '1504215680853-026ed2a45def', '1542282088-72c9c27ed0cd', '1553440569-bcc63803a83d', '1549317661-bd32c8ce0db2', '1511919884226-fd3cad34687c', '1533473359331-0135ef1b58bf']
      },
      природа: {
        prefix: 'nature',
        ids: ['1441974231531-c6227db76b6e', '1470770841072-f978cf4d019e', '1501785888041-af3ef285b470', '1464822759023-fed622ff2c3b', '1472214103451-9374bd1c798e', '1448375240586-882707db888b', '1470252649378-9c29740c9fa8', '1500530855697-b586d89ba3ee']
      },
      тварини: {
        prefix: 'animals',
        ids: ['1474511320723-9a56873867b5', '1560807707-8cc77767d783', '1549366021-9f761d450615', '1552053831-71594a27632d', '1537151608828-ea2b11777ee8', '1517849845537-4d257902454a', '1558788353-f76d92427f16', '1544568100-847a948585b9']
      },
      море: {
        prefix: 'sea',
        ids: ['1507525428034-b723cf961d3e', '1473116763249-2faaef81ccda', '1469474968028-56623f02e42e', '1484291470158-b8f8d608850d', '1439405326854-014607f694d7', '1500534314209-a25ddb2bd429', '1455729552865-3658a5d39692', '1498623116890-37e912163d5d']
      },
      космос: {
        prefix: 'space',
        ids: ['1446776811953-b23d57bd21aa', '1462331940025-496dfbfc7564', '1451187580459-43490279c0fa', '1444703686981-a3abbc4d4fe3', '1419242902214-272b3f66ee7a', '1447433819943-74a20887a81e', '1502134249126-9f3755a50d78', '1534796636912-3b95b3ab5986']
      },
      абстракція: {
        prefix: 'abstract',
        ids: ['1557682250-33bd709cbe85', '1550859492-d5da9d8e45f3', '1541701494587-cb58502866ab', '1549490349-8643362247b5', '1557682224-5b8590cd9ec5', '1558591710-4b4a1ae0f04d', '1574169208507-84376144848b', '1550745165-9bc0b252726f']
      }
    };
    this.categoryOrder.forEach((category, categoryIndex) => {
      photos[category].ids.forEach((photoId, photoIndex) => {
        const id = `sample-${categoryIndex + 1}-${photoIndex + 1}`;
        result[id] = {
          id,
          ownerUserId: 0,
          title: `${this.categoryLabels.uk[category]} ${photoIndex + 1}`,
          category,
          filePath: `samples/${photos[category].prefix}-${photoIndex + 1}.jpg`,
          sourceUrl: `https://images.unsplash.com/photo-${photoId}`,
          sourceLabel: 'Unsplash',
          rating: 0
        };
      });
    });
    return result;
  }

  init() {
    this.bindAuthPanels();
    this.bindAuthForms();
    this.bindNavigation();
    this.bindProfile();
    this.bindFriends();
    this.bindSettings();
    this.bindLibrary();
    this.bindUpload();

    try {
      const saved = localStorage.getItem('wm_user');
      this.user = saved ? JSON.parse(saved) : null;
    } catch {
      localStorage.removeItem('wm_user');
    }

    if (this.user) {
      this.applyLanguage(this.user.language || 'uk');
      this.applyTheme(this.user.theme || 'dark');
      this.onLoginSuccess();
    } else {
      this.applyLanguage(localStorage.getItem('wm_locale') || 'uk');
      this.showAuthScreen();
    }
  }

  text(uk, en) {
    return this.locale === 'uk' ? uk : en;
  }

  showToast(message, error = false) {
    const toast = document.getElementById('toast');
    if (!toast) return;
    toast.textContent = message;
    toast.classList.toggle('error', error);
    toast.classList.remove('hidden');
    clearTimeout(this.toastTimer);
    this.toastTimer = setTimeout(() => toast.classList.add('hidden'), 2600);
  }

  async api(endpoint, options = {}) {
    const response = await fetch(this.apiUrl + endpoint, {
      method: options.method || 'GET',
      headers: { 'Content-Type': 'application/json' },
      body: options.body ? JSON.stringify(options.body) : undefined
    });
    const text = await response.text();
    let data = null;
    try { data = text ? JSON.parse(text) : null; } catch { data = null; }
    if (!response.ok) throw new Error(data?.error || data?.message || text || `HTTP ${response.status}`);
    return data;
  }

  escape(value) {
    const node = document.createElement('div');
    node.textContent = String(value ?? '');
    return node.innerHTML;
  }

  showAuthScreen() {
    document.getElementById('auth-screen')?.classList.remove('hidden');
    document.getElementById('app')?.classList.add('hidden');
  }

  onLoginSuccess() {
    document.getElementById('auth-screen')?.classList.add('hidden');
    document.getElementById('app')?.classList.remove('hidden');
    document.getElementById('header-username').textContent = this.user.username || '';
    this.switchPage('wallpapers');
  }

  applyLanguage(language) {
    this.locale = language === 'en' ? 'en' : 'uk';
    localStorage.setItem('wm_locale', this.locale);
    document.documentElement.lang = this.locale;
    document.querySelectorAll('[data-i18n]').forEach(element => {
      const value = this.translations[this.locale][element.dataset.i18n];
      if (value) element.textContent = value;
    });
    document.querySelectorAll('[data-i18n-placeholder]').forEach(element => {
      const value = this.translations[this.locale][element.dataset.i18nPlaceholder];
      if (value) element.placeholder = value;
    });
    ['auth-lang', 'reg-language', 'app-lang', 'settings-language'].forEach(id => {
      const element = document.getElementById(id);
      if (element) element.value = this.locale;
    });

    // Update tab labels and upload button
    const tabLib = document.getElementById('btn-tab-library');
    if (tabLib) tabLib.textContent = this.translations[this.locale].tabLibrary || tabLib.textContent;
    const tabCol = document.getElementById('btn-tab-collection');
    if (tabCol) tabCol.textContent = this.translations[this.locale].tabCollection || tabCol.textContent;
    const uploadBtn = document.getElementById('btn-upload');
    if (uploadBtn) uploadBtn.textContent = this.translations[this.locale].uploadButton || uploadBtn.textContent;

    // Update upload modal title and buttons
    const uploadTitle = document.querySelector('#upload-modal h3');
    if (uploadTitle) uploadTitle.textContent = this.translations[this.locale].uploadModalTitle || uploadTitle.textContent;
    const uploadCancel = document.getElementById('upload-cancel');
    if (uploadCancel) uploadCancel.textContent = this.translations[this.locale].uploadCancel || uploadCancel.textContent;
    const uploadSubmit = document.getElementById('upload-submit');
    if (uploadSubmit) uploadSubmit.textContent = this.translations[this.locale].uploadSubmit || uploadSubmit.textContent;

    // Update upload select placeholder option
    const uploadSelect = document.getElementById('upload-category');
    if (uploadSelect) {
      const opt = uploadSelect.querySelector('option[value=""]');
      if (opt) opt.textContent = this.translations[this.locale].chooseCategory || opt.textContent;
      // update category option labels to selected locale
      Array.from(uploadSelect.options).forEach(o => {
        if (!o.value) return;
        const label = this.categoryLabels[this.locale] && this.categoryLabels[this.locale][o.value];
        if (label) o.textContent = label;
      });
    }

    // Update category filter buttons
    document.querySelectorAll('.cat-btn').forEach(button => {
      const cat = button.dataset.category;
      if (!cat) return;
      if (cat === 'all') {
        button.textContent = this.translations[this.locale].catAll || button.textContent;
      } else {
        const label = this.categoryLabels[this.locale] && this.categoryLabels[this.locale][cat];
        if (label) button.textContent = label;
      }
    });
  }

  applyTheme(theme) {
    const light = theme === 'light';
    document.body.classList.toggle('theme-light', light);
    document.body.classList.toggle('theme-dark', !light);
    ['theme-toggle', 'settings-theme-toggle'].forEach(id => {
      const element = document.getElementById(id);
      if (element) element.checked = light;
    });
  }

  bindAuthPanels() {
    document.getElementById('show-register')?.addEventListener('click', () => {
      document.getElementById('panel-login').classList.add('hidden');
      document.getElementById('panel-register').classList.remove('hidden');
    });
    document.getElementById('show-login')?.addEventListener('click', () => {
      document.getElementById('panel-register').classList.add('hidden');
      document.getElementById('panel-login').classList.remove('hidden');
    });
  }

  bindAuthForms() {
    document.getElementById('login-form')?.addEventListener('submit', async event => {
      event.preventDefault();
      try {
        const data = await this.api('/login', {
          method: 'POST',
          body: {
            username: document.getElementById('login-username').value.trim(),
            password: document.getElementById('login-password').value
          }
        });
        this.user = data.user;
        this.user.language = document.getElementById('auth-lang').value;
        localStorage.setItem('wm_user', JSON.stringify(this.user));
        this.applyLanguage(this.user.language);
        this.applyTheme(this.user.theme || 'dark');
        this.onLoginSuccess();
      } catch (error) {
        this.showToast(this.text('Неправильний логін або пароль', 'Invalid username or password'), true);
      }
    });

    document.getElementById('register-form')?.addEventListener('submit', async event => {
      event.preventDefault();
      try {
        const data = await this.api('/register', {
          method: 'POST',
          body: {
            username: document.getElementById('reg-username').value.trim(),
            password: document.getElementById('reg-password').value,
            name: document.getElementById('reg-name').value.trim(),
            language: document.getElementById('reg-language').value,
            theme: document.getElementById('reg-theme').value
          }
        });
        this.user = data.user;
        localStorage.setItem('wm_user', JSON.stringify(this.user));
        this.applyLanguage(this.user.language);
        this.applyTheme(this.user.theme);
        this.onLoginSuccess();
      } catch (error) {
        this.showToast(error.message, true);
      }
    });

    document.querySelector('.nav-logout')?.addEventListener('click', () => {
      localStorage.removeItem('wm_user');
      location.reload();
    });
    document.getElementById('reg-theme')?.addEventListener('change', event => this.applyTheme(event.target.value));
  }

  bindNavigation() {
    document.querySelectorAll('.nav-item:not(.nav-logout)').forEach(button => {
      button.addEventListener('click', () => this.switchPage(button.dataset.page));
    });
  }

  switchPage(pageName) {
    document.querySelectorAll('.nav-item').forEach(button => button.classList.toggle('active', button.dataset.page === pageName));
    document.querySelectorAll('.page').forEach(page => {
      const active = page.id === `page-${pageName}`;
      page.classList.toggle('active', active);
      page.classList.toggle('hidden', !active);
    });
    const key = `nav${pageName[0].toUpperCase()}${pageName.slice(1)}`;
    document.getElementById('topbar-title').textContent = this.translations[this.locale][key] || pageName;
    if (pageName === 'wallpapers') document.getElementById('btn-tab-library')?.click();
    if (pageName === 'friends') this.loadFriends();
    if (pageName === 'profile') this.loadProfile();
    if (pageName === 'settings') this.renderSettings();
  }

  bindLibrary() {
    const libraryButton = document.getElementById('btn-tab-library');
    const collectionButton = document.getElementById('btn-tab-collection');
    libraryButton?.addEventListener('click', () => {
      this.toggleLibraryTab(true);
      this.loadWallpapers(this.currentCategory);
    });
    collectionButton?.addEventListener('click', () => {
      this.toggleLibraryTab(false);
      this.loadCollection();
    });
    document.querySelectorAll('.cat-btn').forEach(button => {
      button.addEventListener('click', () => {
        document.querySelectorAll('.cat-btn').forEach(item => item.classList.remove('active'));
        button.classList.add('active');
        this.currentCategory = button.dataset.category;
        this.loadWallpapers(this.currentCategory);
      });
    });
  }

  toggleLibraryTab(libraryActive) {
    document.getElementById('section-library').classList.toggle('hidden', !libraryActive);
    document.getElementById('section-collection').classList.toggle('hidden', libraryActive);
    document.getElementById('btn-tab-library').classList.toggle('active-tab', libraryActive);
    document.getElementById('btn-tab-collection').classList.toggle('active-tab', !libraryActive);
  }

  sortWallpapers(items) {
    return [...items].sort((a, b) => {
      const categoryA = this.categoryOrder.indexOf(a.category);
      const categoryB = this.categoryOrder.indexOf(b.category);
      const safeA = categoryA < 0 ? 999 : categoryA;
      const safeB = categoryB < 0 ? 999 : categoryB;
      return safeA - safeB || String(a.title).localeCompare(String(b.title), this.locale);
    });
  }

  async loadWallpapers(category = 'all') {
    const grid = document.getElementById('library-grid');
    if (!grid || !this.user) return;
    grid.innerHTML = `<p>${this.text('Завантаження...', 'Loading...')}</p>`;
    try {
      const serverItems = await this.api('/wallpapers');
      let items = [...serverItems];
      // Always include sample wallpapers in addition to server items so users keep seeing them
      const existingIds = new Set(items.map(it => String(it.id)));
      Object.values(this.sampleWallpapers).forEach(sample => {
        if (!existingIds.has(String(sample.id))) items.push(sample);
      });
      if (category !== 'all') items = items.filter(item => item.category === category);
      this.renderWallpaperGrid(grid, this.sortWallpapers(items), { save: true, grouped: category === 'all' });
    } catch (error) {
      grid.innerHTML = `<p class="error-text">${this.text('Не вдалося завантажити обої. Перевірте сервер.', 'Could not load wallpapers. Check the server.')}</p>`;
    }
  }

  renderWallpaperGrid(grid, items, options = {}) {
    this.wallpaperCache = { ...this.wallpaperCache };
    if (!items.length) {
      grid.innerHTML = `<div class="empty-state">${this.text('У цій категорії ще немає фото', 'No photos in this category')}</div>`;
      return;
    }
    items.forEach(item => { this.wallpaperCache[item.id] = item; });
    if (!options.grouped) {
      grid.classList.add('pinterest-grid');
      grid.innerHTML = items.map(item => this.createWallpaperCard(item, options.save)).join('');
      return;
    }
    grid.classList.remove('pinterest-grid');
    const groups = new Map();
    items.forEach(item => {
      if (!groups.has(item.category)) groups.set(item.category, []);
      groups.get(item.category).push(item);
    });
    grid.innerHTML = [...groups.entries()].map(([category, group]) => `
      <section class="category-section">
        <h2>${this.escape(this.categoryLabels[this.locale][category] || category)}</h2>
        <div class="pinterest-grid">${group.map(item => this.createWallpaperCard(item, options.save)).join('')}</div>
      </section>
    `).join('');
  }

  collectionKey() {
    return `wm_collection_${this.user.userId}`;
  }

  getCollection() {
    try { return JSON.parse(localStorage.getItem(this.collectionKey()) || '[]'); } catch { return []; }
  }

  saveToCollectionId(id) {
    const wallpaper = this.wallpaperCache[id] || this.sampleWallpapers[id];
    if (!wallpaper) return;
    const collection = this.getCollection();
    if (collection.some(item => String(item.id) === String(wallpaper.id))) {
      this.showToast(this.text('Це фото вже у вашій колекції', 'This photo is already in your collection'));
      return;
    }
    collection.push(wallpaper);
    localStorage.setItem(this.collectionKey(), JSON.stringify(collection));
    this.showToast(this.text('Фото додано до вашої колекції', 'Photo added to your collection'));
  }

  removeFromCollection(id) {
    const collection = this.getCollection().filter(item => String(item.id) !== String(id));
    localStorage.setItem(this.collectionKey(), JSON.stringify(collection));
    this.loadCollection();
  }

  loadCollection() {
    const grid = document.getElementById('collection-grid');
    const items = this.sortWallpapers(this.getCollection());
    items.forEach(item => { this.wallpaperCache[item.id] = item; });
    if (!items.length) {
      grid.innerHTML = `<div class="empty-state"><h3>${this.text('Колекція порожня', 'Collection is empty')}</h3><p>${this.text('Додайте фото з бібліотеки або галереї друга.', 'Add photos from the library or a friend gallery.')}</p></div>`;
      return;
    }
    grid.innerHTML = items.map(item => this.createWallpaperCard(item, false, true)).join('');
  }

  imageUrl(wallpaper) {
    if (wallpaper.filePath && (/^(https?:|data:|\/)/.test(wallpaper.filePath) || wallpaper.filePath.includes('samples/'))) {
      return wallpaper.filePath;
    }
    return `${this.apiUrl}/wallpapers/image/${wallpaper.id}`;
  }

  createWallpaperCard(wallpaper, showSave = false, showRemove = false) {
    const id = this.escape(wallpaper.id);
    const title = this.escape(wallpaper.title || this.text('Без назви', 'Untitled'));
    const category = this.escape(this.categoryLabels[this.locale][wallpaper.category] || wallpaper.category || this.text('Інше', 'Other'));
    const rating = this.getUserRating(wallpaper.id, wallpaper);
    return `
      <article class="pin-card">
        <div class="pin-img-wrap">
          <img src="${this.escape(this.imageUrl(wallpaper))}" alt="${title}" loading="lazy">
          <div class="pin-overlay">
            <h4>${title}</h4>
            <div class="pin-cat">${category}</div>
            ${wallpaper.sourceUrl ? `<a class="photo-source" href="${this.escape(wallpaper.sourceUrl)}" target="_blank" rel="noopener noreferrer">${this.text('Джерело', 'Source')}: ${this.escape(wallpaper.sourceLabel || 'Web')}</a>` : ''}
            <div class="pin-rating" id="rating-${id}">★ ${rating} / 5</div>
            <div class="pin-actions">
              <button class="btn btn-sm btn-secondary" type="button" onclick="window.app.openViewer('${id}')">${this.text('Відкрити', 'Open')}</button>
              ${showSave ? `<button class="btn btn-sm btn-primary" type="button" onclick="window.app.saveToCollectionId('${id}')">${this.text('Забрати собі', 'Save')}</button>` : ''}
              ${showRemove ? `<button class="btn btn-sm btn-danger" type="button" onclick="window.app.removeFromCollection('${id}')">${this.text('Видалити', 'Remove')}</button>` : ''}
            </div>
          </div>
        </div>
      </article>`;
  }

  bindFriends() {
    const search = () => this.searchUsers();
    document.getElementById('btn-search-users')?.addEventListener('click', search);
    document.getElementById('search-users-input')?.addEventListener('keydown', event => {
      if (event.key === 'Enter') search();
    });
  }

  async searchUsers() {
    const query = document.getElementById('search-users-input').value.trim();
    const container = document.getElementById('search-results');
    container.innerHTML = `<p>${this.text('Пошук...', 'Searching...')}</p>`;
    try {
      const users = await this.api(`/users/search?q=${encodeURIComponent(query)}&exclude=${this.user.userId}`);
      if (!users.length) {
        container.innerHTML = `<div class="empty-state">${this.text('Нікого не знайдено', 'No users found')}</div>`;
        return;
      }
      container.innerHTML = users.map(user => `
        <div class="list-item">
          <button class="friend-link" type="button" onclick="window.app.openFriendProfile(${user.id})">
            <strong>${this.escape(user.name)}</strong><span>@${this.escape(user.username)}</span>
          </button>
          <button class="btn btn-sm ${user.isFriend ? 'btn-secondary' : 'btn-primary'}" type="button"
            ${user.isFriend ? `onclick="window.app.openFriendProfile(${user.id})"` : `onclick="window.app.addFriend(${user.id})"`}>
            ${user.isFriend ? this.text('Профіль', 'Profile') : this.text('Додати', 'Add')}
          </button>
        </div>`).join('');
    } catch (error) {
      container.innerHTML = `<p class="error-text">${this.escape(error.message)}</p>`;
    }
  }

  async addFriend(friendId) {
    try {
      await this.api('/friends', { method: 'POST', body: { userId: this.user.userId, friendId } });
      this.showToast(this.text('Друга додано', 'Friend added'));
      await Promise.all([this.loadFriends(), this.searchUsers()]);
    } catch (error) {
      this.showToast(error.message, true);
    }
  }

  async loadFriends() {
    const list = document.getElementById('friends-list');
    list.innerHTML = `<p>${this.text('Завантаження...', 'Loading...')}</p>`;
    try {
      const friends = await this.api(`/friends/${this.user.userId}`);
      if (!friends.length) {
        list.innerHTML = `<div class="empty-state">${this.text('У вас ще немає друзів', 'You have no friends yet')}</div>`;
        return;
      }
      list.innerHTML = friends.map(friend => `
        <button class="friend-card" type="button" onclick="window.app.openFriendProfile(${friend.id})">
          <span class="friend-avatar">${this.escape((friend.name || friend.username).slice(0, 1).toUpperCase())}</span>
          <span><strong>${this.escape(friend.name)}</strong><small>@${this.escape(friend.username)}</small></span>
          <span class="friend-open">${this.text('Галерея', 'Gallery')} →</span>
        </button>`).join('');
    } catch (error) {
      list.innerHTML = `<p class="error-text">${this.escape(error.message)}</p>`;
    }
  }

  async openFriendProfile(friendId) {
    try {
      const [profile, wallpapers] = await Promise.all([
        this.api(`/users/${friendId}`),
        this.api(`/wallpapers/${friendId}`)
      ]);
      this.friendProfile = profile;
      wallpapers.forEach(item => { this.wallpaperCache[item.id] = item; });
      const overlay = document.createElement('div');
      overlay.className = 'profile-modal';
      overlay.innerHTML = `
        <div class="profile-modal-box">
          <button class="modal-close" type="button" aria-label="${this.text('Закрити', 'Close')}">×</button>
          <div class="friend-profile-head">
            <span class="friend-avatar large">${this.escape((profile.name || profile.username).slice(0, 1).toUpperCase())}</span>
            <div><h2>${this.escape(profile.name)}</h2><p>@${this.escape(profile.username)}</p></div>
          </div>
          <h3>${this.text('Галерея друга', 'Friend gallery')}</h3>
          <div class="friend-gallery pinterest-grid">
            ${wallpapers.length
              ? this.sortWallpapers(wallpapers).map(item => this.createWallpaperCard(item, true)).join('')
              : `<div class="empty-state">${this.text('У друга ще немає завантажених фото', 'This friend has no uploaded photos yet')}</div>`}
          </div>
        </div>`;
      document.body.appendChild(overlay);
      const close = () => overlay.remove();
      overlay.querySelector('.modal-close').addEventListener('click', close);
      overlay.addEventListener('click', event => { if (event.target === overlay) close(); });
    } catch (error) {
      this.showToast(error.message, true);
    }
  }

  bindProfile() {
    document.getElementById('edit-profile-form')?.addEventListener('submit', async event => {
      event.preventDefault();
      try {
        const data = await this.api(`/user/${this.user.userId}`, {
          method: 'PUT',
          body: {
            username: document.getElementById('profile-username').value.trim(),
            name: document.getElementById('profile-name').value.trim(),
            language: this.locale
          }
        });
        this.user = { ...this.user, ...data.user };
        localStorage.setItem('wm_user', JSON.stringify(this.user));
        document.getElementById('header-username').textContent = this.user.username;
        this.showToast(this.text('Профіль збережено', 'Profile saved'));
      } catch (error) {
        this.showToast(error.message, true);
      }
    });

    document.getElementById('change-password-form')?.addEventListener('submit', async event => {
      event.preventDefault();
      try {
        await this.api(`/user/${this.user.userId}/password`, {
          method: 'PUT',
          body: {
            currentPassword: document.getElementById('pwd-current').value,
            newPassword: document.getElementById('pwd-new').value
          }
        });
        event.target.reset();
        this.showToast(this.text('Пароль змінено', 'Password changed'));
      } catch (error) {
        this.showToast(error.message, true);
      }
    });
  }

  async loadProfile() {
    document.getElementById('profile-name').value = this.user.name || '';
    document.getElementById('profile-username').value = this.user.username || '';
    try {
      const [friends, wallpapers] = await Promise.all([
        this.api(`/friends/${this.user.userId}`),
        this.api(`/wallpapers/${this.user.userId}`)
      ]);
      document.getElementById('profile-stats').innerHTML = `
        <div class="stat-card"><div class="label">${this.text('Друзі', 'Friends')}</div><div class="value">${friends.length}</div></div>
        <div class="stat-card"><div class="label">${this.text('Завантажені фото', 'Uploaded photos')}</div><div class="value">${wallpapers.length}</div></div>
        <div class="stat-card"><div class="label">${this.text('Моя колекція', 'My collection')}</div><div class="value">${this.getCollection().length}</div></div>`;
    } catch {
      document.getElementById('profile-stats').innerHTML = '';
    }
  }

  bindSettings() {
    ['app-lang', 'settings-language', 'auth-lang'].forEach(id => {
      document.getElementById(id)?.addEventListener('change', event => {
        this.applyLanguage(event.target.value);
        if (this.user) {
          this.user.language = this.locale;
          localStorage.setItem('wm_user', JSON.stringify(this.user));
        }
      });
    });
    ['theme-toggle', 'settings-theme-toggle'].forEach(id => {
      document.getElementById(id)?.addEventListener('change', event => {
        const theme = event.target.checked ? 'light' : 'dark';
        this.applyTheme(theme);
        if (this.user) {
          this.user.theme = theme;
          localStorage.setItem('wm_user', JSON.stringify(this.user));
        }
      });
    });
  }

  renderSettings() {
    document.getElementById('settings-view').innerHTML = `
      <div class="stat-card"><div class="label">${this.text('Користувач', 'User')}</div><div class="value">@${this.escape(this.user.username)}</div></div>
      <div class="stat-card"><div class="label">${this.text('Мова', 'Language')}</div><div class="value">${this.locale.toUpperCase()}</div></div>`;
  }

  bindUpload() {
    const input = document.getElementById('upload-input');
    const modal = document.getElementById('upload-modal');
    let file = null;
    document.getElementById('btn-upload')?.addEventListener('click', () => input.click());
    input?.addEventListener('change', event => {
      file = event.target.files[0] || null;
      if (!file) return;
      document.getElementById('upload-title').value = file.name.replace(/\.[^.]+$/, '');
      document.getElementById('upload-category').value = '';
      modal.classList.remove('hidden');
    });
    document.getElementById('upload-cancel')?.addEventListener('click', () => {
      modal.classList.add('hidden');
      input.value = '';
      file = null;
    });
    document.getElementById('upload-submit')?.addEventListener('click', async () => {
      const category = document.getElementById('upload-category').value;
      if (!file || !category) {
        this.showToast(this.text('Оберіть файл і категорію', 'Choose a file and category'), true);
        return;
      }
      // Skip relying on browser-reported MIME type; server will validate image bytes.
      if (file.size > 10 * 1024 * 1024) {
        this.showToast(this.text('Файл занадто великий (макс 10 МБ)', 'File too large (max 10 MB)'), true);
        return;
      }
      try {
        const data = await this.readFile(file);
        const res = await this.api('/wallpapers/upload', {
          method: 'POST',
          body: {
            ownerUserId: this.user.userId,
            title: document.getElementById('upload-title').value.trim() || file.name,
            category,
            filename: file.name,
            data
          }
        });
        modal.classList.add('hidden');
        input.value = '';
        file = null;
        this.showToast(this.text('Фото завантажено', 'Photo uploaded'));
        // Reload the category that was selected for upload so existing items stay visible
        this.loadWallpapers(category);
      } catch (error) {
        this.showToast(error.message, true);
      }
    });
  }

  readFile(file) {
    return new Promise((resolve, reject) => {
      const reader = new FileReader();
      reader.onload = () => resolve(String(reader.result).split(',')[1] || '');
      reader.onerror = reject;
      reader.readAsDataURL(file);
    });
  }

  openViewer(id) {
    const wallpaper = this.wallpaperCache[id] || this.sampleWallpapers[id];
    if (!wallpaper) return;
    const selectedRating = this.getUserRating(id, wallpaper);
    const overlay = document.createElement('div');
    overlay.className = 'viewer-modal';
    overlay.innerHTML = `
      <div class="viewer-box">
        <button class="modal-close" type="button">×</button>
        <img src="${this.escape(this.imageUrl(wallpaper))}" alt="${this.escape(wallpaper.title)}">
        <h3>${this.escape(wallpaper.title)}</h3>
        <p class="rating-label">${this.text('Ваша оцінка', 'Your rating')}</p>
        <div class="rating-stars" role="group" aria-label="${this.text('Поставити оцінку', 'Set rating')}">
          ${[1, 2, 3, 4, 5].map(value => `
            <button class="rating-star ${value <= selectedRating ? 'active' : ''}" type="button"
              data-rating="${value}" aria-label="${value}">★</button>`).join('')}
        </div>
      </div>`;
    document.body.appendChild(overlay);
    const close = () => overlay.remove();
    overlay.querySelector('.modal-close').addEventListener('click', close);
    overlay.addEventListener('click', event => { if (event.target === overlay) close(); });
    overlay.querySelectorAll('.rating-star').forEach(button => {
      button.addEventListener('click', async () => {
        await this.setWallpaperRating(id, Number(button.dataset.rating), overlay);
      });
    });
  }

  ratingStorageKey() {
    return `wm_ratings_${this.user?.userId || 'guest'}`;
  }

  getStoredRatings() {
    try {
      return JSON.parse(localStorage.getItem(this.ratingStorageKey()) || '{}');
    } catch {
      return {};
    }
  }

  getUserRating(id, wallpaper) {
    const stored = this.getStoredRatings();
    return Number(stored[id] ?? wallpaper.rating) || 0;
  }

  async setWallpaperRating(id, rating, overlay) {
    if (rating < 1 || rating > 5) return;
    const wallpaper = this.wallpaperCache[id] || this.sampleWallpapers[id];
    if (!wallpaper) return;

    try {
      const numericId = Number(id);
      if (Number.isInteger(numericId) && numericId > 0) {
        const data = await this.api(`/wallpapers/${numericId}/rate`, {
          method: 'POST',
          body: { rating }
        });
        wallpaper.rating = data.rating;
      } else {
        wallpaper.rating = rating;
      }

      const stored = this.getStoredRatings();
      stored[id] = rating;
      localStorage.setItem(this.ratingStorageKey(), JSON.stringify(stored));
      this.updateRatingEverywhere(id, rating);
      overlay.querySelectorAll('.rating-star').forEach(button => {
        button.classList.toggle('active', Number(button.dataset.rating) <= rating);
      });
      this.showToast(this.text(`Оцінку ${rating} збережено`, `Rating ${rating} saved`));
    } catch (error) {
      this.showToast(error.message, true);
    }
  }

  updateRatingEverywhere(id, rating) {
    document.querySelectorAll(`[id="rating-${CSS.escape(String(id))}"]`).forEach(element => {
      element.textContent = `★ ${rating} / 5`;
    });

    const collection = this.getCollection();
    let changed = false;
    collection.forEach(item => {
      if (String(item.id) === String(id)) {
        item.rating = rating;
        changed = true;
      }
    });
    if (changed) localStorage.setItem(this.collectionKey(), JSON.stringify(collection));
  }
}

document.addEventListener('DOMContentLoaded', () => {
  window.app = new WallpaperHub();
});
