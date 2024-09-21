import React from "react";
import { Home, Items, User, New, Requests, NewRequest } from "./pages";
import { itemInputs, requestInputs } from "./formSource";
import { BrowserRouter, Routes, Route } from "react-router-dom";
import Login from "./pages/login";

function App() {
  return (
    <BrowserRouter>
      <Routes>
        <Route path="/">
          <Route index element={<Home />} />
          <Route path="/items">
            <Route index element={<Items />} />
            <Route path="new" element={<New inputs={itemInputs} />} />
            <Route path=":itemId" element={<User />} />
          </Route>
          <Route path="/requests">
            <Route index element={<Requests />} />
            <Route path="new" element={<NewRequest inputs={requestInputs} />} />
            <Route path=":itemId" element={<User />} />
          </Route>
          <Route path="/login">
            <Route index element={<Login />} />
          </Route>
        </Route>
      </Routes>
    </BrowserRouter>
  );
}

export default App;
